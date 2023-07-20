/*
 *  gd-tools - a set of programs to enhance goldendict for immersion learning.
 *  Copyright (C) 2023 Ajatt-Tools
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "anki_search.h"
#include "precompiled.h"
#include "util.h"

// reference
// https://camo.githubusercontent.com/192ba92ba3971bbb9eecede21c5f8d3a0a1afd52516af28a199335813e75f243/68747470733a2f2f692e696d6775722e636f6d2f6958335648484f2e706e67
// todo print tags (with links)

using namespace std::string_view_literals;
using json = nlohmann::json;

static constexpr auto ankiconnect_addr{ "127.0.0.1:8765"sv };
static constexpr std::chrono::seconds timeout{ 3U };
static constexpr std::size_t expected_n_fields{ 10 };
static constexpr std::string_view help_text = R"EOF(usage: gd-ankisearch [OPTIONS]

Search your Anki collection and output Note Ids that match query.

OPTIONS
  --field-name NAME    optional field to limit search to.
  --deck-name NAME     optional deck to limit search to.
  --show-fields F1,F2  optional comma-separated list of fields to show.
  --word WORD          required search term

EXAMPLES
gd-ankisearch --field-name VocabKanji --word %GDWORD%
gd-ankisearch --deck-name Mining --word %GDWORD%
)EOF";
static constexpr std::string_view css_style = R"EOF(<style>
.gd-ankisearch-table {
  font-family: arial, sans-serif;
  border-collapse: collapse;
  border-spacing: 0;
  max-width: 100%;
  width: 100%;
  display: block;
  overflow-x: auto;
  overscroll-behavior-inline: contain;
}
.gd-ankisearch-table td,
.gd-ankisearch-table th {
  border: 1px solid #dddddd;
  text-align: left;
  padding: 4px;
}
.gd-ankisearch-table tr:nth-child(odd) {
  background-color: #dddddd55;
}
.gd-ankisearch-table tr:first-child {
  background-color: #00000011;
}
.gd-ankisearch-table tr.new {
  border-bottom: 2px solid #3b82f6;
  background-color: #3b82f611;
}
.gd-ankisearch-table tr.learning {
  border-bottom: 2px solid #dc2626;
  background-color: #dc262611;
}
.gd-ankisearch-table tr.review {
  border-bottom: 2px solid #16a34a;
  background-color: #16a34a11;
}
.gd-ankisearch-table tr.buried {
  border-bottom: 2px solid #858585;
  background-color: #85858511;
}
.gd-ankisearch-table tr.suspended {
  border-bottom: 2px solid #ffe88d;
  background-color: #ffe88d11;
}
.gd-ankisearch-table img {
  max-height: 200px;
  max-width: 200px;
}
.gd-ankisearch-table a {
  color: #1b2c5d;
  text-decoration: none;
}
.gd-ankisearch-table a:hover {
  filter: hue-rotate(-20deg) brightness(120%);
}
</style>
)EOF";

struct card_info
{
  uint64_t id;
  int64_t queue;
  int64_t type;
  std::string deck_name;
  std::map<std::string, std::string> fields;
};

auto split_anki_field_names(std::string_view const show_fields) -> std::vector<std::string>
{
  // Expects a list of Anki fields separated by commas.
  std::vector<std::string> parsed{};
  parsed.reserve(expected_n_fields);
  std::ranges::copy(
    show_fields //
      | std::views::split(',') //
      | std::views::transform([](auto v) { return std::string(v.begin(), v.end()); }), //
    std::back_inserter(parsed)
  );
  return parsed;
}

struct search_params
{
  std::string_view gd_word{};
  std::string_view field_name{};
  std::string_view deck_name{};
  std::vector<std::string> show_fields{};

  void assign(std::string_view const key, std::string_view const value)
  {
    if (key == "--field-name") {
      field_name = value;
    } else if (key == "--deck-name") {
      deck_name = value;
    } else if (key == "--show-fields") {
      show_fields = split_anki_field_names(value);
    } else if (key == "--word") {
      gd_word = value;
    }
  }
};

auto make_find_cards_request_str(search_params const& params) -> std::string
{
  auto request = json::parse(R"EOF({
    "action": "findCards",
    "version": 6,
    "params": {
        "query": "deck:current"
    }
  })EOF");

  std::string query{ params.gd_word };

  if (not params.field_name.empty()) {
    query = fmt::format("\"{}:*{}*\"", params.field_name, query);
  }
  if (not params.deck_name.empty()) {
    query = fmt::format("\"deck:{}\" {}", params.deck_name, query);
  }

  request["params"]["query"] = query;

  return request.dump();
}

auto make_get_media_dir_path_request_str() -> std::string
{
  return R"EOF({
      "action": "getMediaDirPath",
      "version": 6
  })EOF";
}

auto make_ankiconnect_request(std::string_view const request_str) -> cpr::Response
{
  return cpr::Post(
    cpr::Url{ ankiconnect_addr },
    cpr::Body{ request_str },
    cpr::Header{ { "Content-Type", "application/json" } },
    cpr::Timeout{ timeout }
  );
}

auto make_info_request_str(std::vector<uint64_t> const& cids)
{
  auto request = json::parse(R"EOF({
    "action": "cardsInfo",
    "version": 6,
    "params": {
        "cards": []
    }
  })EOF");
  request["params"]["cards"] = cids;
  return request.dump();
}

auto get_cids_info(std::vector<uint64_t> const& cids) -> nlohmann::json
{
  auto const request_str = make_info_request_str(cids);
  cpr::Response const r = make_ankiconnect_request(request_str);
  raise_if(r.status_code != 200, "Couldn't connect to Anki.");
  auto const obj = json::parse(r.text);
  raise_if(not obj["error"].is_null(), "Error getting data from AnkiConnect.");
  return obj["result"];
}

auto find_cids(search_params const& params) -> std::vector<uint64_t>
{
  auto const request_str = make_find_cards_request_str(params);
  cpr::Response const r = make_ankiconnect_request(request_str);
  raise_if(r.status_code != 200, "Couldn't connect to Anki.");
  auto const obj = json::parse(r.text);
  raise_if(not obj["error"].is_null(), "Error getting data from AnkiConnect.");
  return obj["result"];
}

auto fetch_media_dir_path() -> std::string
{
  auto const request_str = make_get_media_dir_path_request_str();
  cpr::Response const r = make_ankiconnect_request(request_str);
  raise_if(r.status_code != 200, "Couldn't connect to Anki.");
  auto const obj = json::parse(r.text);
  raise_if(not obj["error"].is_null(), "Error getting data from AnkiConnect.");
  return obj["result"];
}

void print_table_header(search_params const& params)
{
  // Print the first row (header) that contains <th></th> tags, starting with Card ID.
  fmt::print("<tr>");
  fmt::print("<th>Card ID</th>");
  fmt::print("<th>Deck name</th>");
  for (auto const& field: params.show_fields) { fmt::print("<th>{}</th>", field); }
  fmt::print("</tr>\n");
}

auto card_json_to_obj(nlohmann::json const& card_json) -> card_info
{
  // https://github.com/FooSoft/anki-connect#cardsinfo
  return {
    .id = card_json["cardId"],
    .queue = card_json["queue"],
    .type = card_json["type"],
    .deck_name = card_json["deckName"], //
    .fields =
      [&card_json]() {
        std::map<std::string, std::string> result;
        for (auto const& element: card_json["fields"].items()) {
          result.emplace(element.key(), element.value()["value"]);
        }
        return result;
      }(), //
  };
}

auto gd_format(std::string const& field, std::string const& media_dir) -> std::string
{
  // Make sure GoldenDict displays images correctly by specifying the full path.
  static std::regex const img_re{ "(<img[^<>]*src=\")" };
  static std::regex const any_undesirables{ R"EOF(\[sound:|\]|<[^<>]+>|["'.,!?]+|…|。|、|！|？|　|・|～|\(|\))EOF" };
  auto const link_content = strtrim(std::regex_replace(field, any_undesirables, " "));
  auto const link_text = std::regex_replace(field, img_re, fmt::format("$1file://{}/", media_dir));
  return link_content.empty() ? link_text : fmt::format("<a href=\"ankisearch:{}\">{}</a>", link_content, link_text);
}

void print_cards_info(search_params const& params)
{
  auto const cids = find_cids(params);
  if (cids.empty()) {
    return fmt::print("No cards found.\n");
  }
  auto const media_dir_path = fetch_media_dir_path();
  fmt::print("<table class=\"gd-ankisearch-table\">\n");
  print_table_header(params);
  for (auto const& card: get_cids_info(cids) | std::views::transform(card_json_to_obj)) {
    fmt::print("<tr class=\"{}\">", determine_card_class(card.queue, card.type));
    fmt::print("<td><a href=\"ankisearch:cid:{}\">{}</a></td>", card.id, card.id);
    fmt::print("<td>{}</td>", card.deck_name);
    for (auto const& field: params.show_fields) {
      fmt::print(
        "<td>{}</td>",
        (card.fields.contains(field) and not card.fields.at(field).empty()
           ? gd_format(card.fields.at(field), media_dir_path)
           : "Not present")
      );
    }
    fmt::print("</tr>\n");
  }
  fmt::print("</table>\n");
  fmt::print("{}\n", css_style);
}

void search_anki_cards(std::span<std::string_view const> const args)
{
  try {
    print_cards_info(fill_args<search_params>(args));
  } catch (help_requested const& ex) {
    fmt::print(help_text);
  } catch (runtime_error const& ex) {
    fmt::print("{}\n", ex.what());
  }
}
