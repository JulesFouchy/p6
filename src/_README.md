## Shader

### set()

We declare all the possible signatures instead of using a template because it makes the API clearer and avoids ugly template errors if an invalid type is passed in.
The template is only used internally to avoid code duplication.

### Color

Conceptually alpha is really not part of the color: it is related to blending. And the blending could be done in many different ways.