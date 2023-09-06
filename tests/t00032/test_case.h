/**
 * tests/t00032/test_case.cc
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

TEST_CASE("t00032", "[test-case][class]")
{
    auto [config, db] = load_config("t00032");

    auto diagram = config.diagrams["t00032_class"];

    REQUIRE(diagram->name == "t00032_class");

    auto model = generate_class_diagram(*db, diagram);

    REQUIRE(model->name() == "t00032_class");

    {
        auto puml = generate_class_puml(diagram, *model);
        AliasMatcher _A(puml);

        REQUIRE_THAT(puml, StartsWith("@startuml"));
        REQUIRE_THAT(puml, EndsWith("@enduml\n"));

        REQUIRE_THAT(puml, IsClass(_A("Base")));
        REQUIRE_THAT(puml, IsClass(_A("TBase")));
        REQUIRE_THAT(puml, IsClass(_A("A")));
        REQUIRE_THAT(puml, IsClass(_A("B")));
        REQUIRE_THAT(puml, IsClass(_A("C")));
        REQUIRE_THAT(puml, IsClass(_A("R")));

        REQUIRE_THAT(puml, IsClassTemplate("Overload", "T,L,Ts..."));

        REQUIRE_THAT(puml, IsBaseClass(_A("Base"), _A("Overload<T,L,Ts...>")));
        REQUIRE_THAT(
            puml, IsBaseClass(_A("TBase"), _A("Overload<TBase,int,A,B,C>")));
        REQUIRE_THAT(
            puml, IsBaseClass(_A("A"), _A("Overload<TBase,int,A,B,C>")));
        REQUIRE_THAT(
            puml, IsBaseClass(_A("B"), _A("Overload<TBase,int,A,B,C>")));
        REQUIRE_THAT(
            puml, IsBaseClass(_A("C"), _A("Overload<TBase,int,A,B,C>")));
        REQUIRE_THAT(
            puml, !IsDependency(_A("Overload<TBase,int,A,B,C>"), _A("TBase")));
        REQUIRE_THAT(
            puml, !IsDependency(_A("Overload<TBase,int,A,B,C>"), _A("A")));
        REQUIRE_THAT(
            puml, !IsDependency(_A("Overload<TBase,int,A,B,C>"), _A("B")));
        REQUIRE_THAT(
            puml, !IsDependency(_A("Overload<TBase,int,A,B,C>"), _A("C")));

        save_puml(config.output_directory(), diagram->name + ".puml", puml);
    }
    {
        auto j = generate_class_json(diagram, *model);

        using namespace json;

        REQUIRE(IsBaseClass(j, "A",
            "Overload<clanguml::t00032::TBase,int,clanguml::t00032::A,clanguml:"
            ":t00032::B,clanguml::t00032::C>"));

        save_json(config.output_directory(), diagram->name + ".json", j);
    }
    {
        auto mmd = generate_class_mermaid(diagram, *model);

        save_mermaid(config.output_directory(), diagram->name + ".mmd", mmd);
    }
}