/**
 * tests/t00012/test_case.cc
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

TEST_CASE("t00012", "[test-case][class]")
{
    auto [config, db] = load_config("t00012");

    auto diagram = config.diagrams["t00012_class"];

    REQUIRE(diagram->name == "t00012_class");

    auto model = generate_class_diagram(*db, diagram);

    REQUIRE(model->name() == "t00012_class");

    {
        auto puml = generate_class_puml(diagram, *model);
        AliasMatcher _A(puml);

        REQUIRE_THAT(puml, StartsWith("@startuml"));
        REQUIRE_THAT(puml, EndsWith("@enduml\n"));
        REQUIRE_THAT(puml, IsClassTemplate("A", "T,Ts..."));
        REQUIRE_THAT(puml, IsClassTemplate("B", "int... Is"));

        REQUIRE_THAT(puml, IsInstantiation(_A("B<int... Is>"), _A("B<3,2,1>")));
        REQUIRE_THAT(
            puml, IsInstantiation(_A("B<int... Is>"), _A("B<1,1,1,1>")));
        REQUIRE_THAT(puml,
            IsInstantiation(_A("C<T,int... Is>"),
                _A("C<std::map<int,"
                   "std::vector<std::vector<std::vector<std::string>>>>,3,3,"
                   "3>")));

        save_puml(config.output_directory(), diagram->name + ".puml", puml);
    }
    {
        auto j = generate_class_json(diagram, *model);

        using namespace json;

        REQUIRE(IsClassTemplate(j, "A<T,Ts...>"));
        REQUIRE(IsClassTemplate(j, "B<int... Is>"));
        REQUIRE(IsClass(j, "A<int,std::string,float>"));
        REQUIRE(IsClass(j, "A<int,std::string,bool>"));
        REQUIRE(IsClass(j,
            "C<std::map<int,std::vector<std::vector<std::vector<std::string>>>>"
            ",3,3,3>"));

        save_json(config.output_directory(), diagram->name + ".json", j);
    }
    {
        auto mmd = generate_class_mermaid(diagram, *model);

        save_mermaid(config.output_directory(), diagram->name + ".mmd", mmd);
    }
}