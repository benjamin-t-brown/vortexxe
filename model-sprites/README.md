This folder contains a tool to generate a 2d spritesheet given a 3d .glb object created in TinkerCad.  It uses ThreeJS to load and display the model, then generates an HTML Canvas of the specified rotation sprites, which you can then copy to clipboard and paste into a new image using any image editing software.

To run application:
- Place your .glb model in the "models" directory.
- Run an http server in this directory (ex: https://www.npmjs.com/package/http-server)
- Visit the index.html file via your http server and specify your model in the params:
  * `http://127.0.0.1:8080/?model=<model name without extension>`

Click the Generate button to see changes to your spritesheet.