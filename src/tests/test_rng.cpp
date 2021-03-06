#include "tests.h"
#include "test_rng.h"

#include <botan/libstate.h>
#include <botan/hex.h>
#include <iostream>
#include <fstream>

#if defined(BOTAN_HAS_X931_RNG)
  #include <botan/x931_rng.h>
  #include <botan/aes.h>
  #include <botan/des.h>
#endif

using namespace Botan;

namespace {

RandomNumberGenerator* get_rng(const std::string& algo, const std::string& ikm_hex)
   {
   const auto ikm = hex_decode(ikm_hex);

#if defined(BOTAN_HAS_X931_RNG)
   if(algo == "X9.31-RNG(TripleDES)")
      return new ANSI_X931_RNG(new TripleDES, new Fixed_Output_RNG(ikm));
   else if(algo == "X9.31-RNG(AES-128)")
      return new ANSI_X931_RNG(new AES_128, new Fixed_Output_RNG(ikm));
   else if(algo == "X9.31-RNG(AES-192)")
      return new ANSI_X931_RNG(new AES_192, new Fixed_Output_RNG(ikm));
   else if(algo == "X9.31-RNG(AES-256)")
      return new ANSI_X931_RNG(new AES_256, new Fixed_Output_RNG(ikm));
#endif

   return nullptr;
   }

size_t x931_test(const std::string& algo,
                 const std::string& ikm,
                 const std::string& out,
                 size_t L)
   {
   std::unique_ptr<RandomNumberGenerator> rng(get_rng(algo, ikm));

   if(!rng)
      throw std::runtime_error("Unknown RNG " + algo);

   rng->reseed(0);

   const std::string got = hex_encode(rng->random_vec(L));

   if(got != out)
      {
      std::cout << "X9.31 " << got << " != " << out << "\n";
      return 1;
      }

   return 0;
   }

}

size_t test_rngs()
   {
   std::ifstream vec(TEST_DATA_DIR "/x931.vec");

   return run_tests_bb(vec, "RNG", "Out", true,
             [](std::map<std::string, std::string> m) -> size_t
             {
             return x931_test(m["RNG"], m["IKM"], m["Out"], to_u32bit(m["L"]));
             });
   }
