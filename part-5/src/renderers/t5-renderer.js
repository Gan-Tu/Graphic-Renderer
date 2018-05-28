import THREE from '../lib/three';
import Renderer from '../lib/renderer';
import vert from '../shaders/extra/test.vert';
import frag from '../shaders/extra/test.frag';

class T5Renderer extends Renderer {
  initScene() {
    if (!this.checkShader(vert, frag)) {
      this.setErrorScene();
      return;
    }

    const geometry = new THREE.TorusKnotGeometry(4, 0.5, 35, 20, 2, 5);
    geometry.computeTangents();
    const material = this.createShaderMaterial(vert, frag);

    this.cube = new THREE.Mesh(geometry, material);
    this.scene.add(this.cube);
  }

  update(dt) {
    if (!this.focussed) {
      this.updateCamera(dt / 12000);
    }
  }
}

export default T5Renderer;