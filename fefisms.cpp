#include "fefisms.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <chrono>
#include <thread>

fefisms::fefisms(
  std::string configFile,
  std::mt19937& rng) :
    rng_(rng)
{
  // Load the config file.
  YAML::Node config = YAML::LoadFile(configFile);

  // Set up the verbly database.
  database_ = std::unique_ptr<verbly::database>(
    new verbly::database(config["verbly_datafile"].as<std::string>()));

  // Set up the Twitter client.
  twitter::auth auth;
  auth.setConsumerKey(config["consumer_key"].as<std::string>());
  auth.setConsumerSecret(config["consumer_secret"].as<std::string>());
  auth.setAccessKey(config["access_key"].as<std::string>());
  auth.setAccessSecret(config["access_secret"].as<std::string>());

  client_ = std::unique_ptr<twitter::client>(new twitter::client(auth));
}

void fefisms::run() const
{
  verbly::filter formFilter =
    (verbly::form::complexity == 1)
    && (verbly::form::proper == false);

  // Blacklist ethnic slurs
  verbly::filter cleanFilter =
    !(verbly::word::usageDomains %= (verbly::notion::wnid == 106718862));

  for (;;)
  {
    std::cout << "Generating tweet..." << std::endl;

    verbly::token utterance;

    verbly::inflection nounInfl = verbly::inflection::base;
    verbly::inflection hypoInfl = verbly::inflection::base;

    if (std::bernoulli_distribution(1.0/2.0)(rng_))
    {
      hypoInfl = verbly::inflection::plural;
    }

    if (std::bernoulli_distribution(1.0/2.0)(rng_))
    {
      nounInfl = verbly::inflection::plural;
    }

    verbly::word noun = database_->words(
      (verbly::notion::partOfSpeech == verbly::part_of_speech::noun)
      && (verbly::word::forms(nounInfl) %= formFilter)
      && cleanFilter
      && (verbly::notion::hyponyms %=
        (verbly::word::forms(hypoInfl) %= formFilter)
        && cleanFilter)).first();

    verbly::word hyponym = database_->words(
      (verbly::notion::partOfSpeech == verbly::part_of_speech::noun)
      && (verbly::notion::hypernyms %= noun)
      && cleanFilter
      && (verbly::word::forms(hypoInfl) %= formFilter)).first();

    if (std::bernoulli_distribution(1.0/2.0)(rng_))
    {
      utterance << verbly::token(noun, nounInfl);
      utterance << verbly::token(hyponym, hypoInfl);
    } else {
      utterance << verbly::token(hyponym, hypoInfl);
      utterance << verbly::token(noun, nounInfl);
    }

    std::string action = utterance.compile();
    std::cout << action << std::endl;

    try
    {
      std::cout << "Tweeting..." << std::endl;

      client_->updateStatus(action);

      std::cout << "Success!" << std::endl;
      std::cout << "Waiting..." << std::endl;
    } catch (const twitter::twitter_error& e)
    {
      std::cout << "Error while tweeting: " << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::hours(1));

    std::cout << std::endl;
  }
}
