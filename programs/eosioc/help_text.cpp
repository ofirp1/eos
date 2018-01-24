/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include "help_text.hpp"
#include "localize.hpp"
#include <regex>

using namespace eosio::client::localize;

const char* transaction_help_text_header = _("An error occurred while submitting the transaction for this command!");

const char* duplicate_transaction_help_text = _(R"text(The transaction is a duplicate of one already pushed to the producers.  If this
is an intentionally repeated transaction there are a few ways to resolve the
issue:
  - wait for the next block
  - combine duplicate transactions into a single transaction
  - adjust the expiration time using the `--expiration <milliseconds>` option
  - use the `--force-unique` option to add additional nonce data
    Please note, this will consume more bandwidth than the base transaction )text");

const char* missing_perms_help_text = _(R"text(The transaction requires permissions that were not granted by the transaction.
Missing permission from:
  - ${1}

Please use the `-p,--permissions` option to add the missing accounts!
Note: you will need an unlocked wallet that can authorized these permissions.)text");

const char* missing_sigs_help_text = _(R"text(The transaction requires permissions that could not be authorized by the wallet.
Missing authrizations:
  - ${1}@${2}

Please make sure the proper keys are imported into an unlocked wallet and try again!)text");

const char* missing_scope_help_text = _(R"text(The transaction requires scopes that were not listed by the transaction.
Missing scope(s):
  - ${1}

Please use the `-S,--scope` option to add the missing accounts!)text");


const char* tx_unknown_account_help_text = _("The transaction references an account which does not exist.");

const char* unknown_account_help_text = _(R"text(Unknown accounts:
  - ${1}

Please check the account names and try again!)text");

const char* missing_abi_help_text = _(R"text(The ABI for action "${2}" on code account "${1}" is unknown.
The payload cannot be automatically serialized.

You can push an arbitrary transaction using the 'push transaction' subcommand)text");

const char* unknown_wallet_help_text = _("Unable to find a wallet named \"${1}\", are you sure you typed the name correctly?");

const char* bad_wallet_password_help_text = _("Invalid password for wallet named \"${1}\"");

const char* locked_wallet_help_text = _("The wallet named \"${1}\" is locked.  Please unlock it and try again.");

const char* duplicate_key_import_help_text = _("This key is already imported into the wallet named \"${1}\".");

const char* unknown_abi_table_help_text = _(R"text(The ABI for the code on account "${1}" does not specify table "${2}".

Please check the account and table name, and verify that the account has the expected code using:
  eosc get code ${1})text");

const char* help_regex_error = _("Error locating help text: ${code} ${what}");

const std::vector<std::pair<const char*, std::vector<const char *>>> error_help_text {
   {"Error\n: 3030011", {transaction_help_text_header, duplicate_transaction_help_text}},
   {"Error\n: 3030001[^\\x00]*\\{\"acct\":\"([^\"]*)\"\\}", {transaction_help_text_header, missing_perms_help_text}},
   {"Error\n: 3030002[^\\x00]*Transaction declares authority.*account\":\"([^\"]*)\",\"permission\":\"([^\"]*)\"", {transaction_help_text_header, missing_sigs_help_text}},
   {"Error\n: 3030008[^\\x00]*\\{\"scope\":\"([^\"]*)\"\\}", {transaction_help_text_header, missing_scope_help_text}},
   {"Account not found: ([\\S]*)", {transaction_help_text_header, tx_unknown_account_help_text, unknown_account_help_text}},
   {"Error\n: 303", {transaction_help_text_header}},
   {"unknown key[^\\x00]*abi_json_to_bin.*code\":\"([^\"]*)\".*action\":\"([^\"]*)\"", {missing_abi_help_text}},
   {"unknown key[^\\x00]*chain/get_code.*name\":\"([^\"]*)\"", {unknown_account_help_text}},
   {"Unable to open file[^\\x00]*wallet/open.*postdata\":\"([^\"]*)\"", {unknown_wallet_help_text}},
   {"AES error[^\\x00]*wallet/unlock.*postdata\":\\[\"([^\"]*)\"", {bad_wallet_password_help_text}},
   {"Wallet is locked: ([\\S]*)", {locked_wallet_help_text}},
   {"Key already in wallet[^\\x00]*wallet/import_key.*postdata\":\\[\"([^\"]*)\"", {duplicate_key_import_help_text}},
   {"ABI does not define table[^\\x00]*get_table_rows.*code\":\"([^\"]*)\",\"table\":\"([^\"]*)\"", {unknown_abi_table_help_text}}
};

auto smatch_to_variant(const std::smatch& smatch) {
   auto result = fc::mutable_variant_object();
   for(size_t index = 0; index < smatch.size(); index++) {
      auto name = boost::lexical_cast<std::string>(index);
      if (smatch[index].matched) {
         result = result(name, smatch.str(index));
      } else {
         result = result(name, "");
      }
   }

   return result;
};

const std::map<int64_t, std::string> error_advice = {
        { 3120001, "Name should be less than 13 characters and only contains the following symbol .12345abcdefghijklmnopqrstuvwxyz"},
        { 3120002, "Public key should be encoded in base58 and starts with EOS prefix"},
        { 3120003, "Ensure that your authority JSON follows the following format!\n"
                   "{\n"
                   "  \"threshold\":\"uint32_t\",\n"
                   "  \"keys\":[{ \"key\":\"public_key\", \"weight\":\"uint16_t\" }],\n"
                   "  \"accounts\":[{\n"
                   "    \"permission\":{ \"actor\":\"account_name\", \"permission\":\"permission_name\" },\n"
                   "    \"weight\":\"uint16_t\n"
                   "  }]\n"
                   "}\n"
                   "e.g.\n"
                   "{\n"
                   "  \"threshold\":\"1\",\n"
                   "  \"keys\":[{ \"key\":\"EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV\", \"weight\":\"1\" }],\n"
                   "  \"accounts\":[{\n"
                   "    \"permission\":{ \"actor\":\"initb\", \"permission\":\"social\" },\n"
                   "    \"weight\":\"1\n"
                   "  }]\n"
                  "}"},
        { 3120004, "Ensure that your action JSON follows the contract's abi!" },
        { 3120005, "Ensure that your transaction JSON follows the following format!\n"
                   "{\n"
                   "  \"ref_block_num\":\"uint16_t\",\n"
                   "  \"ref_block_prefix\":\"uint32_t\",\n"
                   "  \"expiration\":\"YYYY-MM-DDThh:mm\",\n"
                   "  \"region\": \"uint16_t\",\n"
                   "  \"read_scope\":[ \"account_name\" ],\n"
                   "  \"write_scope\":[ \"account_name\" ],\n"
                   "  \"actions\":[{ \n"
                   "    \"account\":\"account_name\",\n"
                   "    \"name\":\"action_name\",\n"
                   "    \"authorization\":[{ \"actor\":\"account_name\",\"permission\":\"permission_name\" }],\n"
                   "    \"data\":\"bytes\"\n"
                   "  }]\n"
                   "}"
                   "e.g.\n"
                   "{\n"
                   "  \"ref_block_num\":\"1000\",\n"
                   "  \"ref_block_prefix\":\"3463702842\",\n"
                   "  \"expiration\":\"2018-01-23T01:51:05\",\n"
                   "  \"region\": \"0\",\n"
                   "  \"read_scope\":[ \"initb\", \"initc\" ],\n"
                   "  \"write_scope\":[ \"initb\", \"initc\" ],\n"
                   "  \"actions\":[{ \n"
                   "    \"account\":\"eosio\",\n"
                   "    \"name\":\"transfer\",\n"
                   "    \"authorization\":[{ \"actor\":\"initb\",\"permission\":\"active\" }],\n"
                   "    \"data\":\"000000008093dd74000000000094dd74e80300000000000000\"\n"
                   "  }]\n"
                   "}"},
        { 3120006, "Ensure that your abi JSON follows the following format!\n"
                   "{\n"
                   "  \"types\" : [{ \"new_type_name\":\"type_name\", \"type\":\"type_name\" }],\n"
                   "  \"structs\" : [{ \"name\":\"type_name\", \"base\":\"type_name\", \"fields\": [{ \"name\":\"field_name\", \"type\": \"type_name\" }] }],\n"
                   "  \"actions\" : [{ \"name\":\"action_name\",\"type\":\"type_name\"}],\n"
                   "  \"tables\" : [{\n"
                   "    \"name\":\"table_name\",\n"
                   "    \"index_type\":\"type_name\",\n"
                   "    \"key_names\":[ \"field_name\" ],\n"
                   "    \"key_types\":[ \"type_name\" ],\n"
                   "    \"type\":\"type_name\" "
                   "  }]\n"
                   "}\n"
                   "e.g.\n"
                   "{\n"
                   "  \"types\" : [{ \"new_type_name\":\"account_name\", \"type\":\"name\" }],\n"
                   "  \"structs\" : [\n"
                   "    { \"name\":\"foo\", \"base\":\"\", \"fields\": [{ \"name\":\"by\", \"type\": \"account_name\" }] },\n "
                   "    { \"name\":\"foobar\", \"base\":\"\", \"fields\": [{ \"name\":\"by\", \"type\": \"account_name\" }] }\n"
                   "  ],\n"
                   "  \"actions\" : [{ \"name\":\"foo\",\"type\":\"foo\"}],\n"
                   "  \"tables\" : [{\n"
                   "    \"name\":\"foobar_table\",\n"
                   "    \"index_type\":\"i64\",\n"
                   "    \"key_names\":[ \"by\" ],\n"
                   "    \"key_types\":[ \"account_name\" ],\n"
                   "    \"type\":\"foobar\" "
                   "  }]\n"
                   "}"}
};


namespace eosio { namespace client { namespace help {
bool print_recognized_error_code(const fc::exception& e) {
   // eos recognized error code is from 3000000 to 3999999
   // refer to libraries/chain/include/eosio/chain/exceptions.hpp
   if (e.code() >= 3000000 && e.code() <= 3999999) {
      std::string advice, explanation;

      // Get advice, if any
      const auto advice_itr = error_advice.find(e.code());
      if (advice_itr != error_advice.end()) advice = advice_itr->second;

      // Get explanation from log, if any
      for (auto &log : e.get_log()) {
         // Check if there's a log to display
         if (!log.get_format().empty()) {
            // Localize the message as needed
            explanation += "\n  " + localized_with_variant(log.get_format().data(), log.get_data());
         }
      }
      if (!explanation.empty()) explanation = std::string("Error Details:") + explanation;

      std::cerr << "\033[31m" << "Error " << e.code() << ": " << e.what() << "\033[0m";
      if (!advice.empty()) std::cerr << "\n" << "\033[32m" << advice << "\033[0m";
      if (!explanation.empty()) std::cerr  << "\n" << "\033[33m" << explanation << "\033[0m" << std::endl;
      return true;
   }
   return false;
}

bool print_help_text(const fc::exception& e) {
   // Check if the exception has recognized error code
   if (print_recognized_error_code(e)) return true;
   bool result = false;
   // Large input strings to std::regex can cause SIGSEGV, this is a known bug in libstdc++.
   // See https://stackoverflow.com/questions/36304204/%D0%A1-regex-segfault-on-long-sequences
   auto detail_str = e.to_detail_string();
   // 2048 nice round number. Picked for no particular reason. Bug above was reported for 22K+ strings.
   if (detail_str.size() > 2048) return result;
   try {
      for (const auto& candidate : error_help_text) {
         auto expr = std::regex {candidate.first};
         std::smatch matches;
         if (std::regex_search(detail_str, matches, expr)) {
            auto args = smatch_to_variant(matches);
            for (const auto& msg: candidate.second) {
               std::cerr << localized_with_variant(msg, args) << std::endl;
            }               
            result = true;
            break;
         }
      }
   } catch (const std::regex_error& e ) {
      std::cerr << localized(help_regex_error, ("code", e.code())("what", e.what())) << std::endl;
   }

   return result;   
}

}}}