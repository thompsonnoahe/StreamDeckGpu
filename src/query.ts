const require = createRequire(import.meta.url);
import { createRequire } from 'node:module';

const gpuAddon = require('gpu-metrics.node');

const query = new gpuAddon.GpuQuery();

export default query;
