# `read_json_cached`

Reads JSON from a file and caches the value to avoid repeated file system reads. See the `read_json` node documentation for input, output, and settings  base functionality information. See below for caching specific logic documentation.

## Inputs

### `read_once`

If true, the file will only be read once. Useful if the file is not changing on disk often. Can be used in conjunction with `read_on_frame`

### `read_on_frame`

If true, the file will only be read on the specified `read_frame`. Can be used in conjunction with `read_once`.

### `read_frame`

The frame to read on, if `read_on_frame` is set to true.

## Outputs

See the `read_json` node output documentation.