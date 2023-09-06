/**
 * tests/t00008/test_case.cc
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

TEST_CASE("t00008", "[test-case][class]")
{
    auto [config, db] = load_config("t00008");

    auto diagram = config.diagrams["t00008_class"];

    REQUIRE(diagram->name == "t00008_class");

    auto model = generate_class_diagram(*db, diagram);

    REQUIRE(model->name() == "t00008_class");

    {
        auto puml = generate_class_puml(diagram, *model);
        AliasMatcher _A(puml);

        REQUIRE_THAT(puml, StartsWith("@startuml"));
        REQUIRE_THAT(puml, EndsWith("@enduml\n"));
        // TODO: add option to resolve using declared types
        // REQUIRE_THAT(puml, IsClassTemplate("A", "T, P, bool (*)(int, int),
        // int N"));
        REQUIRE_THAT(puml, IsClassTemplate("A", "T,P=T,CMP=nullptr,int N=3"));
        REQUIRE_THAT(puml, IsClassTemplate("B", "T,C<>"));

        REQUIRE_THAT(puml, (IsField<Public>("value", "T")));
        REQUIRE_THAT(puml, (IsField<Public>("pointer", "T *")));
        REQUIRE_THAT(puml, (IsField<Public>("reference", "T &")));
        REQUIRE_THAT(puml, (IsField<Public>("values", "std::vector<P>")));
        REQUIRE_THAT(puml, (IsField<Public>("ints", "std::array<int,N>")));
        // TODO: add option to resolve using declared types
        // REQUIRE_THAT(puml, IsField(Public("bool (*)(int, int) comparator")));
        REQUIRE_THAT(puml, (IsField<Public>("comparator", "CMP")));

        REQUIRE_THAT(puml, !IsClass(_A("E::nested_template")));
        REQUIRE_THAT(puml, IsClassTemplate("E::nested_template", "ET"));
        REQUIRE_THAT(puml, IsClassTemplate("E::nested_template", "char"));
        REQUIRE_THAT(puml,
            IsInstantiation(
                _A("E::nested_template<ET>"), _A("E::nested_template<char>")));

        save_puml(config.output_directory(), diagram->name + ".puml", puml);
    }
    {
        auto j = generate_class_json(diagram, *model);

        using namespace json;

#if LLVM_VERSION_MAJOR >= 16
        REQUIRE(IsClassTemplate(j, "A<T,P=T,CMP=nullptr,int N=3>"));
#else
        REQUIRE(IsClassTemplate(
            j, "A<T,P=T,clanguml::t00008::CMP=nullptr,int N=3>"));
#endif
        REQUIRE(IsClassTemplate(j, "E::nested_template<ET>"));
        REQUIRE(IsClass(j, "E::nested_template<char>"));

        save_json(config.output_directory(), diagram->name + ".json", j);
    }
    {
        auto mmd = generate_class_mermaid(diagram, *model);

        save_mermaid(config.output_directory(), diagram->name + ".mmd", mmd);
    }
}