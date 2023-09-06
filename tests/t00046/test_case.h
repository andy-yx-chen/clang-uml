/**
 * tests/t00046/test_case.cc
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

TEST_CASE("t00046", "[test-case][class]")
{
    auto [config, db] = load_config("t00046");

    auto diagram = config.diagrams["t00046_class"];

    REQUIRE(diagram->name == "t00046_class");

    auto model = generate_class_diagram(*db, diagram);

    REQUIRE(model->name() == "t00046_class");

    {
        auto puml = generate_class_puml(diagram, *model);
        AliasMatcher _A(puml);

        REQUIRE_THAT(puml, StartsWith("@startuml"));
        REQUIRE_THAT(puml, EndsWith("@enduml\n"));
        REQUIRE_THAT(puml, IsClass(_A("A")));
        REQUIRE_THAT(puml, IsClass(_A("B")));
        REQUIRE_THAT(puml, IsClass(_A("C")));
        REQUIRE_THAT(puml, IsClass(_A("D")));
        REQUIRE_THAT(puml, IsClass(_A("E")));
        REQUIRE_THAT(puml, IsClass(_A("R")));

        REQUIRE_THAT(puml, IsField<Public>("i", "std::vector<std::uint8_t>"));

        save_puml(config.output_directory(), diagram->name + ".puml", puml);
    }
    {
        auto j = generate_class_json(diagram, *model);

        using namespace json;

        REQUIRE(get_element(j, "A").value()["type"] == "class");
        REQUIRE(get_element(j, "AA").value()["type"] == "class");
        REQUIRE(get_element(j, "ns1::A").value()["type"] == "class");
        REQUIRE(get_element(j, "ns1::ns2::D").value()["type"] == "class");

        save_json(config.output_directory(), diagram->name + ".json", j);
    }
    {
        auto mmd = generate_class_mermaid(diagram, *model);

        save_mermaid(config.output_directory(), diagram->name + ".mmd", mmd);
    }
}