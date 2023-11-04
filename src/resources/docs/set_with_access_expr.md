# `set_with_access_expr`

Sets a value specified by the access expr to a desired value. This works for both arrays and objects and combination of those types chained. Designed for ease of use in manipulating data read in from JSON files, but works for any Bifrost object or array. For arrays, ensure the types are compatible. For example, you cannot input a `String` value when trying to set a value inside an `array<float>`

## Inputs

### `obj_or_arr`

The input to set `value` as specified by the access expression.

### `access_expr`

An expression inputted in the form of a string. The values inside the access operators will be set, allowing for chaining like in other programming languages. Values inside square brackets like `[2]` are used to set in arrays and values inside curly brackets like `{some_obj_key}`are used to set in objects.

### `value`

The desired value for the object key or array index as specified by `access_expr`

### `force_to_any_array`

Since this node was designed to work with the `read_json` operator, sometimes it will be required to cast the intermediate values of array retrieval/setting to `array<any>`, if it was done so by `read_json` (please see its documentation for explanation on when this happens). The compound will cast arrays to the `any` type internally if this option is set to `true`. For example, if retrieving `{json_data}[4][2]{foo}` and the intermediate 2d array is of type `any` (perhaps because of mixed data types in the JSON arrays), you will need to enable this option to correctly retrieve the final key `foo`. See the internal Sticky Note for full explanation as well.

## Outputs

### `out_obj_or_array`

The output after the specified key or array index has been set to `value`. When multiple accessors are chained the input `obj_or_arr`'s root will still be returned.

### `success`

Whether setting was successful or not. Could also receive the error message from trying to retrieve the value to be set (see `get_with_access_expr`)

### `msg_if_failed`

An error message explaining why the setting failed. Empty String if `success` is `true`.