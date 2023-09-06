/**
 * tests/t00025/test_case.cc
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

TEST_CASE("t00025", "[test-case][class]")
{
    auto [config, db] = load_config("t00025");

    auto diagram = config.diagrams["t00025_class"];

    REQUIRE(diagram->name == "t00025_class");

    auto model = generate_class_diagram(*db, diagram);

    REQUIRE(model->name() == "t00025_class");

    {
        auto puml = generate_class_puml(diagram, *model);
        AliasMatcher _A(puml);

        REQUIRE_THAT(puml, StartsWith("@startuml"));
        REQUIRE_THAT(puml, EndsWith("@enduml\n"));
        REQUIRE_THAT(puml, IsClass(_A("Target1")));
        REQUIRE_THAT(puml, IsClass(_A("Target2")));
        REQUIRE_THAT(puml, IsClassTemplate("Proxy", "T"));
        REQUIRE_THAT(
            puml, IsInstantiation(_A("Proxy<T>"), _A("Proxy<Target1>")));
        REQUIRE_THAT(
            puml, IsInstantiation(_A("Proxy<T>"), _A("Proxy<Target2>")));
        REQUIRE_THAT(puml,
            IsAggregation(_A("ProxyHolder"), _A("Proxy<Target1>"), "+proxy1"));
        REQUIRE_THAT(puml,
            IsAggregation(_A("ProxyHolder"), _A("Proxy<Target2>"), "+proxy2"));
        REQUIRE_THAT(
            puml, !IsAggregation(_A("ProxyHolder"), _A("Target1"), "+proxy1"));
        REQUIRE_THAT(
            puml, !IsAggregation(_A("ProxyHolder"), _A("Target2"), "+proxy2"));
        REQUIRE_THAT(puml, IsDependency(_A("Proxy<Target1>"), _A("Target1")));
        REQUIRE_THAT(puml, IsDependency(_A("Proxy<Target2>"), _A("Target2")));

        save_puml(config.output_directory(), diagram->name + ".puml", puml);
    }
    {
        auto j = generate_class_json(diagram, *model);

        using namespace json;

        REQUIRE(IsClass(j, "Target1"));
        REQUIRE(IsClass(j, "Target2"));
        REQUIRE(IsClassTemplate(j, "Proxy<T>"));
        REQUIRE(IsDependency(j, "Proxy<clanguml::t00025::Target1>", "Target1"));
        REQUIRE(IsDependency(j, "Proxy<clanguml::t00025::Target2>", "Target2"));

        save_json(config.output_directory(), diagram->name + ".json", j);
    }
    {
        auto mmd = generate_class_mermaid(diagram, *model);

        save_mermaid(config.output_directory(), diagram->name + ".mmd", mmd);
    }
}