/**
 * tests/t20002/test_case.cc
 *
 * Copyright (c) 2021-2023 Bartek Kryza <bkryza@gmail.com>
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

TEST_CASE("t20002", "[test-case][sequence]")
{
    auto [config, db] = load_config("t20002");

    auto diagram = config.diagrams["t20002_sequence"];

    REQUIRE(diagram->name == "t20002_sequence");

    auto model = generate_sequence_diagram(*db, diagram);

    REQUIRE(model->name() == "t20002_sequence");

    {
        auto puml = generate_sequence_puml(diagram, *model);
        AliasMatcher _A(puml);

        REQUIRE_THAT(puml, StartsWith("@startuml"));
        REQUIRE_THAT(puml, EndsWith("@enduml\n"));

        REQUIRE_THAT(puml, HasCall(_A("m1()"), _A("m2()"), ""));
        REQUIRE_THAT(puml, HasCall(_A("m2()"), _A("m3()"), ""));
        REQUIRE_THAT(puml, HasCall(_A("m3()"), _A("m4()"), ""));

        save_puml(config.output_directory(), diagram->name + ".puml", puml);
    }

    {
        auto j = generate_sequence_json(diagram, *model);

        using namespace json;

        REQUIRE(IsFunctionParticipant(j, "m1()"));
        REQUIRE(IsFunctionParticipant(j, "m2()"));
        REQUIRE(IsFunctionParticipant(j, "m3()"));
        REQUIRE(IsFunctionParticipant(j, "m4()"));

        std::vector<int> messages = {FindMessage(j, "m1()", "m2()", ""),
            FindMessage(j, "m2()", "m3()", ""),
            FindMessage(j, "m3()", "m4()", "")};

        REQUIRE(std::is_sorted(messages.begin(), messages.end()));

        save_json(config.output_directory(), diagram->name + ".json", j);
    }
}
