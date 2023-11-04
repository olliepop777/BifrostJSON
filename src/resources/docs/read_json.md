# `read_json`

Reads JSON from a file or input string, and translates it to a Bifrost compatible data type. The two types are Bifrost Object or String. If parsing to a Bifrost Object, the JSON data will be stored at a key at the top level called `json_data`

## Type Translation

The following primitive types will be translated from JSON type -> Bifrost Type

* `boolean` -> `bool`
* `string` -> `string`
* `number_integer` -> `long`
* `number_unsigned` -> `long`
* `number_float` -> `double`
* `null` -> `(bool)false` by default, or optionally `(string)"null"`

## Inputs

### `json_file`

The file path to read JSON data from. This will not be used if `json_str` is specified.

### `json_str`

JSON data in string form. This takes priority over `json_file` if it is not empty.

### `output_a_bifrost_object`

Whether to output an Amino Object representing the JSON data at the output port `out_json_obj`.

### `output_a_json_str`

Whether to output the parsed JSON as a Bifrost String at the output port `out_json_str`. If the input `json_str` is provided, no JSON parsing will happen and `json_str` will just be passed through to `out_json_str`. If `output_a_bifrost_object` is enabled, the JSON will still be parsed to Bifrost Object, but the input `json_str` will be passed through unaltered.

### `null_to_string`

JSON has a datatype called `null` which is not native to Bifrost. By default a `null` JSON value will be converted to the boolean `false` in Bifrost. If this parameter is enabled then `null` will be converted to the Bifrost String `"null"`

### `force_any_arrays`

This overrides all arrays to be of the type `array<any>`, regardless of whether better type deduction could have been made.

By default, this node will do it's best job to deduce the types of arrays. If an array only contains integers for example, it will be parsed to the Bifrost `array<long>` type. Since JSON can contain an infinite number of nested arrays with mixed data types, there is a hard limit on this type deduction. Therefore, if the data types are mixed or the array nesting is greater than a 3D array, `array<any>` will be used as a fallback type. Take for example, the following two arrays:

* `array_A`: `[101, 202, 303, 404]`
* `array_B`: `[101, "cat", 88.9, true]`

With `force_any_arrays` disabled, only `array_B` will become an `array<any>` because it has mixed datatypes. If `force_any_arrays` is enabled, then all arrays, including `array_A` will be overriden to be an `array<any>`

### `debug`

If debug mode is on, the parsed JSON will be printed to the output console (note this is not the same as Maya's Script Editor). This is the parsed representation that the internal JSON library sees, not the final Bifrost translation.

### `print_indent_level`

When printing to Bifrost String or the console, set the indentation level. Default is 4.

## Outputs

### `out_json_obj`

The Bifrost Object representation of the JSON data.

### `out_json_str`

The Bifrost String representation of the JSON data.

### `success`

Whether parsing was successful or not.

### `msg_if_failed`

An error message explaining why the parsing failed. Empty String if `success` is `true`