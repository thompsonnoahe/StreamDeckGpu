import {
  action,
  WillAppearEvent,
  streamDeck,
  DidReceiveSettingsEvent,
  JsonObject,
} from '@elgato/streamdeck';
import { Gpu } from '../types/gpu';
import Vendor from '../types/vendor';
import ActionWithChart, { Settings } from '../types/action';

@action({ UUID: 'com.nthompson.gpu.mem' })
export class GpuMemoryUsage extends ActionWithChart<GpuMemoryUsageSettings> {
  calculatePercentage(gpu: Gpu): number {
    const percentage = (gpu.usedMemory / gpu.memory) * 100;

    return percentage;
  }

  startTimer(gpu: Gpu, action: any, settings: GpuMemoryUsageSettings): void {
    if (this.timers.has(action.id)) {
      clearInterval(this.timers.get(action.id));
    }

    this.timers.set(
      action.id,
      setInterval(() => {
        if (gpu === undefined) {
          streamDeck.logger.error('GPU not found or selected');
          return;
        }

        if (gpu.usedMemory === -1 || gpu.memory === -1) {
          action.showAlert();
          return;
        }

        const asPercent = settings.showAsPercentage === 'true';

        if (asPercent) {
          const percentage = this.calculatePercentage(gpu);
          action.setTitle(`${Math.round(percentage)}%`);
        } else {
          const text = this.formatUsedMemory(gpu);
          action.setTitle(text);
        }
      }, 1000)
    );
  }

  formatUsedMemory(gpu: Gpu) {
    switch (gpu.vendor) {
      case Vendor.Amd: // AMD returns MB, convert to GB
        return `${Math.round(gpu.usedMemory) / Math.pow(1024, 2)} / \n ${
          Math.round(gpu.memory) / Math.pow(1024, 2)
        } GB`;
      case Vendor.Nvidia: // NVIDIA returns bytes, convert to GB
      default:
        return `${(gpu.usedMemory / Math.pow(1024, 3)).toFixed(
          1
        )} / \n ${Math.round(gpu.memory / Math.pow(1024, 3))} GB`;
    }
  }

  override onDidReceiveSettings(
    ev: DidReceiveSettingsEvent<GpuMemoryUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    this.startTimer(gpu!, ev.action, ev.payload.settings);
  }

  override onWillAppear(
    ev: WillAppearEvent<GpuMemoryUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    this.startTimer(gpu!, ev.action, ev.payload.settings);
  }
}

type GpuMemoryUsageSettings = {
  gpuId: string;
  showAsPercentage: string;
} & Settings &
  JsonObject;
