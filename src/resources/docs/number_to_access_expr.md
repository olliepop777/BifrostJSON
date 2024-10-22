# `number_to_access_expr`

Concatenate on either side of a number value the array or object accessor strings, either `{}` or `[]`

## Inputs

### `prev_expr`

If a string is inputted as an existing accessor expression, the new expression will be appended to this

### `number`

A number value to be converted to a string and surrounded by accessor strings

### `is_array`

If `true`, the `[]` will be appended instead of `{}`

## Outputs

### `access_expr`

The output expression.
