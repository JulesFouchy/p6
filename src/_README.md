## Shader

### set()

We declare all the possible signatures instead of using a template because it makes the API clearer and avoids ugly template errors if an invalid type is passed in.
The template is only used internally to avoid code duplication.

### Color

Conceptually alpha is really not part of the color: it is related to blending. And the blending could be done in many different ways.

### Image

We have two constructors for Image: one where data is nullptr, and one where data has some value submitted by the user. We don't merge these into one constructor with a default parameter because we want to have different default values for TextureLayout when we create an empty image to draw onto it VS when we load an image from a file.