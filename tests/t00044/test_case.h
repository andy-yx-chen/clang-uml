/**
 * tests/t00044/test_case.cc
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

TEST_CASE("t00044", "[test-case][class]")
{
    auto [config, db] = load_config("t00044");

    auto diagram = config.diagrams["t00044_class"];

    REQUIRE(diagram->name == "t00044_class");
    REQUIRE(diagram->generate_packages() == true);

    auto model = generate_class_diagram(*db, diagram);

    REQUIRE(model->name() == "t00044_class");

    {
        auto src = generate_class_puml(diagram, *model);
        AliasMatcher _A(src);

        REQUIRE_THAT(src, StartsWith("@startuml"));
        REQUIRE_THAT(src, EndsWith("@enduml\n"));

        REQUIRE_THAT(src, !Contains("type-parameter-"));

        REQUIRE_THAT(src, IsClassTemplate("sink", "T"));
        REQUIRE_THAT(src, IsClassTemplate("signal_handler", "T,A"));

        REQUIRE_THAT(src, IsClassTemplate("signal_handler", "Ret(Args...),A"));
        REQUIRE_THAT(src, IsClassTemplate("signal_handler", "void(int),bool"));

        REQUIRE_THAT(
            src, IsClassTemplate("sink", "signal_handler<Ret(Args...),A>"));

        REQUIRE_THAT(src,
            IsInstantiation(
                _A("sink<T>"), _A("sink<signal_handler<Ret(Args...),A>>")));

        REQUIRE_THAT(src,
            IsInstantiation(_A("sink<signal_handler<Ret(Args...),A>>"),
                _A("sink<signal_handler<void(int),bool>>")));

        REQUIRE_THAT(src, IsClassTemplate("signal_handler", "T,A"));
        REQUIRE_THAT(src,
            IsInstantiation(_A("signal_handler<T,A>"),
                _A("signal_handler<Ret(Args...),A>")));

        REQUIRE_THAT(src,
            IsInstantiation(_A("signal_handler<Ret(Args...),A>"),
                _A("signal_handler<void(int),bool>")));

        save_puml(config.output_directory(), diagram->name + ".puml", src);
    }
    {
        auto j = generate_class_json(diagram, *model);

        using namespace json;

        REQUIRE(IsClassTemplate(j, "sink<T>"));
        REQUIRE(IsClassTemplate(j, "signal_handler<T,A>"));
        REQUIRE(IsClassTemplate(j, "signal_handler<Ret(Args...),A>"));
        REQUIRE(IsStruct(j, "signal_handler<Ret(Args...),A>"));
        REQUIRE(IsClassTemplate(j, "signal_handler<void(int),bool>"));
        REQUIRE(IsClassTemplate(j, "sink<signal_handler<Ret(Args...),A>>"));
        REQUIRE(IsStruct(j, "R"));

        save_json(config.output_directory(), diagram->name + ".json", j);
    }
    {
        auto src = generate_class_mermaid(diagram, *model);

        mermaid::AliasMatcher _A(src);

        REQUIRE_THAT(src, !Contains("type-parameter-"));

        REQUIRE_THAT(src, IsClass(_A("sink<T>")));
        REQUIRE_THAT(src, IsClass(_A("signal_handler<T,A>")));

        REQUIRE_THAT(src, IsClass(_A("signal_handler<Ret(Args...),A>")));
        REQUIRE_THAT(src, IsClass(_A("signal_handler<void(int),bool>")));

        REQUIRE_THAT(src, IsClass(_A("sink<signal_handler<Ret(Args...),A>>")));

        REQUIRE_THAT(src,
            IsInstantiation(
                _A("sink<T>"), _A("sink<signal_handler<Ret(Args...),A>>")));

        REQUIRE_THAT(src,
            IsInstantiation(_A("sink<signal_handler<Ret(Args...),A>>"),
                _A("sink<signal_handler<void(int),bool>>")));

        REQUIRE_THAT(src, IsClass(_A("signal_handler<T,A>")));
        REQUIRE_THAT(src,
            IsInstantiation(_A("signal_handler<T,A>"),
                _A("signal_handler<Ret(Args...),A>")));

        REQUIRE_THAT(src,
            IsInstantiation(_A("signal_handler<Ret(Args...),A>"),
                _A("signal_handler<void(int),bool>")));

        save_mermaid(config.output_directory(), diagram->name + ".mmd", src);
    }
}