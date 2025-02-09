const require = createRequire(import.meta.url);
import { createRequire } from 'node:module';

const gpuAddon = require('../../build/Release/gpu-metrics.node');

const query = new gpuAddon.GpuQuery();

export default query;