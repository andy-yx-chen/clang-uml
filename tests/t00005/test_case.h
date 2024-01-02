/**
 * tests/t00005/test_case.cc
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

TEST_CASE("t00005", "[test-case][class]")
{
    auto [config, db] = load_config("t00005");

    auto diagram = config.diagrams["t00005_class"];

    REQUIRE(diagram->name == "t00005_class");

    auto model = generate_class_diagram(*db, diagram);

    REQUIRE(model->name() == "t00005_class");

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
        REQUIRE_THAT(puml, IsClass(_A("F")));
        REQUIRE_THAT(puml, IsClass(_A("G")));
        REQUIRE_THAT(puml, IsClass(_A("H")));
        REQUIRE_THAT(puml, IsClass(_A("I")));
        REQUIRE_THAT(puml, IsClass(_A("J")));
        REQUIRE_THAT(puml, IsClass(_A("K")));
        REQUIRE_THAT(puml, IsClass(_A("R")));

        REQUIRE_THAT(puml, (IsField<Public>("some_int", "int")));
        REQUIRE_THAT(puml, (IsField<Public>("some_int_pointer", "int *")));
        REQUIRE_THAT(
            puml, (IsField<Public>("some_int_pointer_pointer", "int **")));

        REQUIRE_THAT(puml, IsAggregation(_A("R"), _A("A"), "+a"));
        REQUIRE_THAT(puml, IsAssociation(_A("R"), _A("B"), "+b"));
        REQUIRE_THAT(puml, IsAssociation(_A("R"), _A("C"), "+c"));
        REQUIRE_THAT(puml, IsAssociation(_A("R"), _A("D"), "+d"));
        REQUIRE_THAT(puml, IsAssociation(_A("R"), _A("E"), "+e"));
        REQUIRE_THAT(puml, IsAggregation(_A("R"), _A("F"), "+f"));
        REQUIRE_THAT(puml, IsAssociation(_A("R"), _A("G"), "+g"));
        REQUIRE_THAT(puml, IsAssociation(_A("R"), _A("H"), "+h"));
        REQUIRE_THAT(puml, IsAssociation(_A("R"), _A("I"), "+i"));
        REQUIRE_THAT(puml, IsAssociation(_A("R"), _A("J"), "+j"));
        REQUIRE_THAT(puml, IsAssociation(_A("R"), _A("K"), "+k"));

        save_puml(config.output_directory(), diagram->name + ".puml", puml);
    }

    {
        auto j = generate_class_json(diagram, *model);

        using namespace json;

        REQUIRE(IsClass(j, "A"));
        REQUIRE(IsClass(j, "B"));
        REQUIRE(IsClass(j, "C"));
        REQUIRE(IsClass(j, "D"));
        REQUIRE(IsClass(j, "E"));
        REQUIRE(IsClass(j, "F"));
        REQUIRE(IsClass(j, "G"));
        REQUIRE(IsClass(j, "H"));
        REQUIRE(IsClass(j, "I"));
        REQUIRE(IsClass(j, "J"));
        REQUIRE(IsClass(j, "K"));
        REQUIRE(IsClass(j, "R"));

        REQUIRE(IsAggregation(j, "R", "A", "a"));
        REQUIRE(IsAssociation(j, "R", "B", "b"));
        REQUIRE(IsAssociation(j, "R", "C", "c"));
        REQUIRE(IsAssociation(j, "R", "D", "d"));
        REQUIRE(IsAssociation(j, "R", "E", "e"));
        REQUIRE(IsAggregation(j, "R", "F", "f"));
        REQUIRE(IsAssociation(j, "R", "G", "g"));
        REQUIRE(IsAssociation(j, "R", "H", "h"));
        REQUIRE(IsAssociation(j, "R", "I", "i"));
        REQUIRE(IsAssociation(j, "R", "J", "j"));
        REQUIRE(IsAssociation(j, "R", "K", "k"));

        save_json(config.output_directory(), diagram->name + ".json", j);
    }
    {
        auto mmd = generate_class_mermaid(diagram, *model);
        mermaid::AliasMatcher _A(mmd);

        REQUIRE_THAT(mmd, StartsWith("classDiagram"));
        REQUIRE_THAT(mmd, IsClass(_A("A")));
        REQUIRE_THAT(mmd, IsClass(_A("B")));
        REQUIRE_THAT(mmd, IsClass(_A("C")));
        REQUIRE_THAT(mmd, IsClass(_A("D")));
        REQUIRE_THAT(mmd, IsClass(_A("E")));
        REQUIRE_THAT(mmd, IsClass(_A("F")));
        REQUIRE_THAT(mmd, IsClass(_A("G")));
        REQUIRE_THAT(mmd, IsClass(_A("H")));
        REQUIRE_THAT(mmd, IsClass(_A("I")));
        REQUIRE_THAT(mmd, IsClass(_A("J")));
        REQUIRE_THAT(mmd, IsClass(_A("K")));
        REQUIRE_THAT(mmd, IsClass(_A("R")));

        REQUIRE_THAT(mmd, (IsField<Public>("some_int", "int")));
        REQUIRE_THAT(mmd, (IsField<Public>("some_int_pointer", "int *")));
        REQUIRE_THAT(
            mmd, (IsField<Public>("some_int_pointer_pointer", "int **")));

        REQUIRE_THAT(mmd, IsAggregation(_A("R"), _A("A"), "+a"));
        REQUIRE_THAT(mmd, IsAssociation(_A("R"), _A("B"), "+b"));
        REQUIRE_THAT(mmd, IsAssociation(_A("R"), _A("C"), "+c"));
        REQUIRE_THAT(mmd, IsAssociation(_A("R"), _A("D"), "+d"));
        REQUIRE_THAT(mmd, IsAssociation(_A("R"), _A("E"), "+e"));
        REQUIRE_THAT(mmd, IsAggregation(_A("R"), _A("F"), "+f"));
        REQUIRE_THAT(mmd, IsAssociation(_A("R"), _A("G"), "+g"));
        REQUIRE_THAT(mmd, IsAssociation(_A("R"), _A("H"), "+h"));
        REQUIRE_THAT(mmd, IsAssociation(_A("R"), _A("I"), "+i"));
        REQUIRE_THAT(mmd, IsAssociation(_A("R"), _A("J"), "+j"));
        REQUIRE_THAT(mmd, IsAssociation(_A("R"), _A("K"), "+k"));

        save_mermaid(config.output_directory(), diagram->name + ".mmd", mmd);
    }
}