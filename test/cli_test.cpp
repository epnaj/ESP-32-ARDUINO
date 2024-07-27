#include <iostream>
#include <cli_interface.hpp>

void testMapParsingArgumentStandalone() {
    ps::list <ps::string> args({
        "command", "-argument_standalone"
    });
    auto parsedMap = cli.parseListToMap(args);
    assert(parsedMap.find("-argument_standalone") != std::end(parsedMap));
}

void testMapParsingSingleArgument() {
    ps::list <ps::string> args({
        "command", "-single_argument", "some_data"
    });
    auto parsedMap = cli.parseListToMap(args);
    assert(parsedMap["-single_argument"] == "some_data");
}

void testMapParsingSingleArgumentAndStandalone_1() {
    ps::list <ps::string> args({
        "command", "-single_argument", "some_data", "argument_standalone"
    });
    auto parsedMap = cli.parseListToMap(args);
    assert(parsedMap["-single_argument"] == "some_data");
    assert(parsedMap.find("-argument_standalone") != std::end(parsedMap));
}

void testMapParsingSingleArgumentAndStandalone_2() {
    ps::list <ps::string> args({
        "command",  "-argument_standalone", "-single_argument", "some_data"
    });
    auto parsedMap = cli.parseListToMap(args);
    assert(parsedMap["-single_argument"] == "some_data");
    assert(parsedMap.find("-argument_standalone") != std::end(parsedMap));
}

void testMapParsingManyArguments() {
    ps::list <ps::string> args({
        "command", "-single_argument", "some_data", "-anoteher_single_argument", "second_data"
    });
    auto parsedMap = cli.parseListToMap(args);
        assert(parsedMap["-single_argument"] == "some_data");
        assert(parsedMap["-anoteher_single_argument"] == "second_data");
}

int main() {
    testMapParsingArgumentStandalone();
    testMapParsingSingleArgument();
    testMapParsingSingleArgumentAndStandalone_1();
    testMapParsingSingleArgumentAndStandalone_2();
    testMapParsingManyArguments();

    return 0;
}