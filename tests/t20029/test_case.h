/**
 * tests/t20029/test_case.h
 *
 * Copyright (c) 2021-2024 Bartek Kryza <bkryza@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

TEST_CASE("t20029", "[test-case][sequence]")
{
    auto [config, db] = load_config("t20029");

    auto diagram = config.diagrams["t20029_sequence"];

    REQUIRE(diagram->name == "t20029_sequence");

    auto model = generate_sequence_diagram(*db, diagram);

    REQUIRE(model->name() == "t20029_sequence");
    {
        auto src = generate_sequence_puml(diagram, *model);
        AliasMatcher _A(src);

        REQUIRE_THAT(src, StartsWith("@startuml"));
        REQUIRE_THAT(src, EndsWith("@enduml\n"));

        // Check if all calls exist
        REQUIRE_THAT(
            src, HasCall(_A("tmain()"), _A("ConnectionPool"), "connect()"));
        REQUIRE_THAT(src,
            HasCallInControlCondition(_A("tmain()"),
                _A("Encoder<Retrier<ConnectionPool>>"),
                "send(std::string &&)"));

        REQUIRE_THAT(src,
            HasCall(_A("Encoder<Retrier<ConnectionPool>>"),
                _A("Encoder<Retrier<ConnectionPool>>"),
                "encode(std::string &&)"));

        REQUIRE_THAT(src,
            HasCall(_A("Encoder<Retrier<ConnectionPool>>"),
                _A("encode_b64(std::string &&)"), ""));

        REQUIRE_THAT(src,
            HasCallInControlCondition(_A("Retrier<ConnectionPool>"),
                _A("ConnectionPool"), "send(const std::string &)"));

        REQUIRE_THAT(src,
            !HasCall(
                _A("ConnectionPool"), _A("ConnectionPool"), "connect_impl()"));

        REQUIRE_THAT(src,
            HasMessageComment(_A("tmain()"),
                "Establish connection to the\\n"
                "remote server synchronously"));

        REQUIRE_THAT(src,
            HasMessageComment(_A("tmain()"),
                "Repeat for each line in the\\n"
                "input stream"));

        REQUIRE_THAT(src,
            HasMessageComment(_A("Encoder<Retrier<ConnectionPool>>"),
                "Encode the message using\\n"
                "Base64 encoding and pass\\n"
                "it to the next layer"));

        REQUIRE_THAT(src,
            HasMessageComment(_A("Retrier<ConnectionPool>"),
                "Repeat until send\\(\\) succeeds\\n"
                "or retry count is exceeded"));

        save_puml(config.output_directory(), diagram->name + ".puml", src);
    }

    {
        auto j = generate_sequence_json(diagram, *model);

        using namespace json;

        REQUIRE(!j["participants"].is_null());

        std::vector<int> messages = {
            FindMessage(j, "tmain()", "ConnectionPool", "connect()"),
            FindMessage(j, "tmain()", "Encoder<Retrier<ConnectionPool>>",
                "send(std::string &&)")};

        REQUIRE(std::is_sorted(messages.begin(), messages.end()));

        save_json(config.output_directory(), diagram->name + ".json", j);
    }

    {
        auto src = generate_sequence_mermaid(diagram, *model);

        mermaid::SequenceDiagramAliasMatcher _A(src);
        using mermaid::HasCall;
        using mermaid::HasCallInControlCondition;
        using mermaid::HasMessageComment;

        REQUIRE_THAT(
            src, HasCall(_A("tmain()"), _A("ConnectionPool"), "connect()"));
        REQUIRE_THAT(src,
            HasCallInControlCondition(_A("tmain()"),
                _A("Encoder<Retrier<ConnectionPool>>"),
                "send(std::string &&)"));

        REQUIRE_THAT(src,
            HasCall(_A("Encoder<Retrier<ConnectionPool>>"),
                _A("Encoder<Retrier<ConnectionPool>>"),
                "encode(std::string &&)"));

        REQUIRE_THAT(src,
            HasCall(_A("Encoder<Retrier<ConnectionPool>>"),
                _A("encode_b64(std::string &&)"), ""));

        REQUIRE_THAT(src,
            HasCallInControlCondition(_A("Retrier<ConnectionPool>"),
                _A("ConnectionPool"), "send(const std::string &)"));

        REQUIRE_THAT(src,
            !HasCall(
                _A("ConnectionPool"), _A("ConnectionPool"), "connect_impl()"));

        REQUIRE_THAT(src,
            HasMessageComment(_A("tmain()"),
                "Establish connection to the<br/>"
                "remote server synchronously"));

        REQUIRE_THAT(src,
            HasMessageComment(_A("tmain()"),
                "Repeat for each line in the<br/>"
                "input stream"));

        REQUIRE_THAT(src,
            HasMessageComment(_A("Encoder<Retrier<ConnectionPool>>"),
                "Encode the message using<br/>"
                "Base64 encoding and pass<br/>"
                "it to the next layer"));

        REQUIRE_THAT(src,
            HasMessageComment(_A("Retrier<ConnectionPool>"),
                "Repeat until send\\(\\) succeeds<br/>"
                "or retry count is exceeded"));

        save_mermaid(config.output_directory(), diagram->name + ".mmd", src);
    }
}