/**
 * tests/t00047/test_case.h
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

TEST_CASE("t00047", "[test-case][class]")
{
    auto [config, db] = load_config("t00047");

    auto diagram = config.diagrams["t00047_class"];

    REQUIRE(diagram->name == "t00047_class");

    auto model = generate_class_diagram(*db, diagram);

    REQUIRE(model->name() == "t00047_class");

    {
        auto puml = generate_class_puml(diagram, *model);
        AliasMatcher _A(puml);

        REQUIRE_THAT(puml, StartsWith("@startuml"));
        REQUIRE_THAT(puml, EndsWith("@enduml\n"));

        // Check if class templates exist
        REQUIRE_THAT(puml, IsClassTemplate("conditional_t", "Ts..."));
        REQUIRE_THAT(puml, IsClassTemplate("conditional_t", "Else"));
        REQUIRE_THAT(puml,
            IsClassTemplate("conditional_t", "std::true_type,Result,Tail..."));
        REQUIRE_THAT(puml,
            IsClassTemplate("conditional_t", "std::false_type,Result,Tail..."));

        save_puml(
            config.output_directory() + "/" + diagram->name + ".puml", puml);
    }
    {
        auto j = generate_class_json(diagram, *model);

        using namespace json;

        REQUIRE(IsClassTemplate(j, "conditional_t<Ts...>"));
        REQUIRE(IsClass(j, "conditional_t<Else>"));
        REQUIRE(IsClass(j, "conditional_t<std::true_type,Result,Tail...>"));
        REQUIRE(IsClass(j, "conditional_t<std::false_type,Result,Tail...>"));

        save_json(config.output_directory() + "/" + diagram->name + ".json", j);
    }
}