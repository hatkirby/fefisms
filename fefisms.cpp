#include <verbly.h>
#include <twitter.h>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <chrono>
#include <random>
#include <thread>

int main(int argc, char** argv)
{
  std::random_device random_device;
  std::mt19937 random_engine{random_device()};
  
  if (argc != 2)
  {
    std::cout << "usage: fefisms [configfile]" << std::endl;
    return -1;
  }

  std::string configfile(argv[1]);
  YAML::Node config = YAML::LoadFile(configfile);

  twitter::auth auth;
  auth.setConsumerKey(config["consumer_key"].as<std::string>());
  auth.setConsumerSecret(config["consumer_secret"].as<std::string>());
  auth.setAccessKey(config["access_key"].as<std::string>());
  auth.setAccessSecret(config["access_secret"].as<std::string>());

  twitter::client client(auth);

  verbly::database database(config["verbly_datafile"].as<std::string>());

  verbly::filter formFilter =
    (verbly::form::complexity == 1)
    && (verbly::form::proper == false);
  
  verbly::filter cleanFilter =
    !(verbly::word::usageDomains %= (verbly::notion::wnid == 106718862)); // Blacklist ethnic slurs

  for (;;)
  {
    std::cout << "Generating tweet..." << std::endl;

    verbly::token utterance;

    verbly::inflection nounInfl = verbly::inflection::base;
    verbly::inflection hypoInfl = verbly::inflection::base;
    
    if (std::bernoulli_distribution(1.0/2.0)(random_engine))
    {
      hypoInfl = verbly::inflection::plural;
    }

    if (std::bernoulli_distribution(1.0/2.0)(random_engine))
    {
      nounInfl = verbly::inflection::plural;
    }

    verbly::word noun = database.words(
      (verbly::notion::partOfSpeech == verbly::part_of_speech::noun)
      && (verbly::word::forms(nounInfl) %= formFilter)
      && cleanFilter
      && (verbly::notion::hyponyms %=
        (verbly::word::forms(hypoInfl) %= formFilter)
        && cleanFilter)).first();

    verbly::word hyponym = database.words(
      (verbly::notion::partOfSpeech == verbly::part_of_speech::noun)
      && (verbly::notion::hypernyms %= noun)
      && cleanFilter
      && (verbly::word::forms(hypoInfl) %= formFilter)).first();

    if (std::bernoulli_distribution(1.0/2.0)(random_engine))
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

      client.updateStatus(action);

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

