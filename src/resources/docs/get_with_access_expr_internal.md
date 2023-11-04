# `get_with_access_expr_internal`

The core logic to retrieve values from arrays or objects with the `access_expr`. This internal version has a few extra parameters not meant to be used under normal use. The extra outputs help when re-using logic for `set_with_access_expr`. Only inputs/outputs not found in `get_with_access_expr` are documented here. Please see the related compound `get_with_acces_expr`'s documentation for the full documentation

## Inputs

### `set_value_mode`

When true, outputs the parameters required when used with `set_with_access_expr`

### `token_arr_in`

If empty, generate the token_arr internally. If not empty then use the input value. Useful to re-use the value for `set_with_access_expr`

### `success_in`

If false, skip the entire compound. Could be false for example in `set_with_access_expr` if the access expression is invalid

## Outputs

### `token_arr_out`

Array of tokens used to represent the `access_expr`. Token array is produced by the operator `get_property_access_tokens`

### `last_token_out`

The last token in the array if `set_value_mode` is `true`. The `token_arr_out` will be sliced to be an array of all tokens except the last, and the last token will be output through this port instead. Empty object if `set_value_mode` is `false`

### `collected_any_values`

The successive retrieved values as type `any` for chained accessors like `{key1}[2]{key2}[3]`, mostly used for setting the value up the hierarchy of accessor inside `set_with_access_expr`