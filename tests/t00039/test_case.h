/**
 * tests/t00039/test_case.cc
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

TEST_CASE("t00039", "[test-case][class]")
{
    auto [config, db] = load_config("t00039");

    auto diagram = config.diagrams["t00039_class"];

    REQUIRE(diagram->name == "t00039_class");
    REQUIRE(diagram->generate_packages() == false);
    auto model = generate_class_diagram(*db, diagram);

    REQUIRE(model->name() == "t00039_class");

    {
        auto src = generate_class_puml(diagram, *model);
        AliasMatcher _A(src);

        REQUIRE_THAT(src, StartsWith("@startuml"));
        REQUIRE_THAT(src, EndsWith("@enduml\n"));

        REQUIRE_THAT(src, IsClass(_A("A")));
        REQUIRE_THAT(src, IsClass(_A("AA")));
        REQUIRE_THAT(src, IsClass(_A("AAA")));
        REQUIRE_THAT(src, IsClass(_A("ns2::AAAA")));
        REQUIRE_THAT(src, IsBaseClass(_A("A"), _A("AA")));
        REQUIRE_THAT(src, IsBaseClass(_A("AA"), _A("AAA")));
        REQUIRE_THAT(src, IsBaseClass(_A("AAA"), _A("ns2::AAAA")));
        REQUIRE_THAT(src, !IsClass(_A("detail::AA")));

        REQUIRE_THAT(src, !IsClass(_A("B")));
        REQUIRE_THAT(src, !IsClass(_A("ns1::BB")));

        REQUIRE_THAT(src, IsClass(_A("C")));
        REQUIRE_THAT(src, IsClass(_A("D")));
        REQUIRE_THAT(src, IsClass(_A("E")));
        REQUIRE_THAT(src, IsBaseClass(_A("C"), _A("CD")));
        REQUIRE_THAT(src, IsBaseClass(_A("D"), _A("CD")));
        REQUIRE_THAT(src, IsBaseClass(_A("D"), _A("DE")));
        REQUIRE_THAT(src, IsBaseClass(_A("E"), _A("DE")));
        REQUIRE_THAT(src, IsBaseClass(_A("C"), _A("CDE")));
        REQUIRE_THAT(src, IsBaseClass(_A("D"), _A("CDE")));
        REQUIRE_THAT(src, IsBaseClass(_A("E"), _A("CDE")));

        REQUIRE_THAT(src, IsClassTemplate("ns3::F", "T"));
        REQUIRE_THAT(src, IsClassTemplate("ns3::FF", "T,M"));
        REQUIRE_THAT(src, IsClassTemplate("ns3::FE", "T,M"));
        REQUIRE_THAT(src, IsClassTemplate("ns3::FFF", "T,M,N"));

        save_puml(config.output_directory(), diagram->name + ".puml", src);
    }
    {
        auto j = generate_class_json(diagram, *model);

        using namespace json;

        REQUIRE(IsClass(j, "A"));
        REQUIRE(IsClass(j, "AA"));
        REQUIRE(IsClass(j, "AAA"));
        REQUIRE(IsBaseClass(j, "C", "CD"));
        REQUIRE(IsBaseClass(j, "D", "CD"));
        REQUIRE(IsBaseClass(j, "E", "DE"));
        REQUIRE(IsBaseClass(j, "D", "DE"));
        REQUIRE(IsBaseClass(j, "C", "CDE"));
        REQUIRE(IsBaseClass(j, "D", "CDE"));
        REQUIRE(IsBaseClass(j, "E", "CDE"));

        REQUIRE(IsClassTemplate(j, "ns3::F<T>"));

        save_json(config.output_directory(), diagram->name + ".json", j);
    }
    {
        auto src = generate_class_mermaid(diagram, *model);

        mermaid::AliasMatcher _A(src);

        REQUIRE_THAT(src, IsClass(_A("A")));
        REQUIRE_THAT(src, IsClass(_A("AA")));
        REQUIRE_THAT(src, IsClass(_A("AAA")));
        REQUIRE_THAT(src, IsClass(_A("ns2::AAAA")));
        REQUIRE_THAT(src, IsBaseClass(_A("A"), _A("AA")));
        REQUIRE_THAT(src, IsBaseClass(_A("AA"), _A("AAA")));
        REQUIRE_THAT(src, IsBaseClass(_A("AAA"), _A("ns2::AAAA")));
        REQUIRE_THAT(src, !IsClass(_A("detail::AA")));

        REQUIRE_THAT(src, !IsClass(_A("B")));
        REQUIRE_THAT(src, !IsClass(_A("ns1::BB")));

        REQUIRE_THAT(src, IsClass(_A("C")));
        REQUIRE_THAT(src, IsClass(_A("D")));
        REQUIRE_THAT(src, IsClass(_A("E")));
        REQUIRE_THAT(src, IsBaseClass(_A("C"), _A("CD")));
        REQUIRE_THAT(src, IsBaseClass(_A("D"), _A("CD")));
        REQUIRE_THAT(src, IsBaseClass(_A("D"), _A("DE")));
        REQUIRE_THAT(src, IsBaseClass(_A("E"), _A("DE")));
        REQUIRE_THAT(src, IsBaseClass(_A("C"), _A("CDE")));
        REQUIRE_THAT(src, IsBaseClass(_A("D"), _A("CDE")));
        REQUIRE_THAT(src, IsBaseClass(_A("E"), _A("CDE")));

        REQUIRE_THAT(src, IsClass(_A("ns3::F<T>")));
        REQUIRE_THAT(src, IsClass(_A("ns3::FF<T,M>")));
        REQUIRE_THAT(src, IsClass(_A("ns3::FE<T,M>")));
        REQUIRE_THAT(src, IsClass(_A("ns3::FFF<T,M,N>")));

        save_mermaid(config.output_directory(), diagram->name + ".mmd", src);
    }
}
