import { GpuMetrics } from "@thompsonnoahe/macos-metrics";
import { Gpu } from "../types/gpu";
import Vendor from "../types/vendor";
import { exec } from "child_process";

export default function getMacOSMetrics(samplePeriod: number = 0): Gpu {
  const metrics = GpuMetrics.getMetrics(samplePeriod);
  return {
    deviceId: metrics.id,
    name: metrics.name,
    usage: metrics.gpuUtilization * 100,
    temperature: metrics.temperature,
    usedMemory: undefined,
    memory: undefined,
    power: metrics.powerUsage,
    vendor: Vendor.Apple,
    launchAssociatedApp: function (): void {
      exec('open -a "Activity Monitor"', (error, stdout, stderr) => {
        if (error) {
          console.error(`exec error: ${error}`);
          return;
        }
        console.log(`stdout: ${stdout}`);
        console.error(`stderr: ${stderr}`);
      });
    },
    index: 0,
  };
}
