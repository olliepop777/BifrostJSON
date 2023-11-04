# `get_with_access_expr`

Easily retrieve an object key or array index with a written expression. Designed to be used with JSON data but can be used on any Bifrost Object or array. Mostly useful when chaining multiple accessors in nested data. For example, an expression like `[8]{id_data}{name_data}{first_name}` would retrieve the 8th index of an array containing an object with the nested keys `id_data`, `name_data`, `first_name`

## Inputs

### `obj_or_arr`

The object or array to retrieve from

### `access_expr`

An expression inputted in the form of a string. The values inside the access operators will be retrieved, allowing for chaining like in other programming languages. Values inside square brackets like `[2]` are used to get from arrays and values inside curly brackets like `{some_obj_key}`are used to retrieve from objects.

### `default_and_type`

The type of the final value to retrieve. Also the default value if the index or key doesn't exist in the object or array, or there is some other failure retrieving the value.

### `force_to_any_array`

Since this node was designed to work with the `read_json` operator, sometimes it will be required to cast the intermediate values of array retrievals to `array<any>`, if it was done so by `read_json` (please see its documentation for explanation on when this happens). The compound will cast arrays to the `any` type internally if this option is set to `true`. For example, if retrieving `{json_data}[4][2]{foo}` and the intermediate 2d array is of type `any` (perhaps because of mixed data types in the JSON arrays), you will need to enable this option to correctly retrieve the final key `foo`. See the internal Sticky Note for full explanation as well.

## Outputs

### `out_value`

The final value requested through the `access_expr`.

### `success`

Whether retrieval was successful or not.

### `msg_if_failed`

An error message explaining why the retrieval failed. Empty String if `success` is `true`.