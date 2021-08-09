/*
Authored by Suka Isnaini on 9th of August 2021
Created for PT Sejahtera Empati Pratama
All Copyrights belong to PT Sejahtera Empati Pratama
*/

#ifndef JSONSTRING_HPP
#define JSONSTRING_HPP
#include "nlohmann/json.hpp"
/*
{
    "$schema": "http://json-schema.org/draft-07/schema",
    "title": "The root schema",
    "required": [
        "status",
        "pitch1",
        "pitch2",
        "pitch3",
        "pitch4"
    ],
    "properties": {
        "pitch4": {
            "title": "The pitch4 schema",
            "type": "number",
            "description": " Pitch 4 value for the pitch analysis.",
            "$id": "#/properties/pitch4",
            "examples": [
                8.3
            ],
            "default": 0.0
        },
        "pitch1": {
            "title": "The pitch1 schema",
            "type": "number",
            "description": "Pitch 1 value for the pitch analysis.",
            "$id": "#/properties/pitch1",
            "examples": [
                2.1
            ],
            "default": 0.0
        },
        "status": {
            "title": "The status schema",
            "type": "number",
            "description": "Error code for pitch analysis.",
            "$id": "#/properties/status",
            "examples": [
                0
            ],
            "default": 0.0
        },
        "pitch3": {
            "title": "The pitch3 schema",
            "type": "number",
            "description": " Pitch 3 value for the pitch analysis.",
            "$id": "#/properties/pitch3",
            "examples": [
                6.7
            ],
            "default": 0.0
        },
        "pitch2": {
            "title": "The pitch2 schema",
            "type": "number",
            "description": " Pitch 2 value for the pitch analysis.",
            "$id": "#/properties/pitch2",
            "examples": [
                4.6
            ],
            "default": 0.0
        }
    },
    "type": "object",
    "description": "The root schema comprises the entire JSON document.",
    "$id": "http://example.com/example.json",
    "examples": [
        {
            "pitch4": 8.3,
            "pitch1": 2.1,
            "status": 0,
            "comment":"success",
            "pitch3": 6.7,
            "pitch2": 4.6
        }
    ],
    "additionalProperties": true,
    "default": {}
}
*/

nlohmann::json static jsonResult =
    R"({"pitch4":8.3,"pitch1":2.1,"status":0,"comment":0,"pitch3":6.7,"pitch2":4.6})"_json;

#endif  // JSONSTRING_HPP
