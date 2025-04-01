import {
  action,
  PropertyInspectorDidAppearEvent,
  SingletonAction,
  WillAppearEvent,
  streamDeck,
  DidReceiveSettingsEvent,
  KeyDownEvent,
  WillDisappearEvent,
} from '@elgato/streamdeck';
import query from '../query';
import { Gpu } from '../types/gpu';
import Vendor from '../types/vendor';

@action({ UUID: 'com.nthompson.gpu-metrics.mem' })
export class GpuMemoryUsage extends SingletonAction<GpuMemoryUsageSettings> {
  timers: Map<string, NodeJS.Timeout> = new Map();
  query = query;
  devices = this.query.getGpus();

  getGpu(gpuId: string): Gpu | undefined {
    return this.devices.find((gpu: Gpu) => gpu.deviceId === gpuId);
  }

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
    let memoryUnit: string;
    switch (gpu.vendor) {
      case Vendor.Nvidia:
        memoryUnit = 'bytes';
        break;
      case Vendor.Amd:
        memoryUnit = 'MB';
        break;
      default:
        memoryUnit = 'bytes';
    }

    if (memoryUnit === 'bytes') {
      return `${(gpu.usedMemory / Math.pow(1024, 3)).toFixed(
        1
      )} / \n ${Math.round(gpu.memory / Math.pow(1024, 3))} GB`;
    } else {
      return `${Math.trunc(gpu.usedMemory) / Math.pow(1024, 2)} / \n ${
        Math.trunc(gpu.memory) / Math.pow(1024, 2)
      } GB`;
    }
  }

  override onDidReceiveSettings(
    ev: DidReceiveSettingsEvent<GpuMemoryUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    this.startTimer(gpu!, ev.action, ev.payload.settings);
  }

  override onPropertyInspectorDidAppear(
    ev: PropertyInspectorDidAppearEvent<GpuMemoryUsageSettings>
  ): Promise<void> | void {
    const gpus = this.devices.map((gpu: Gpu) => {
      return {
        title: "#" + gpu.index + " " + gpu.name,
        value: gpu.deviceId,
      };
    });

    streamDeck.ui.current?.sendToPropertyInspector(gpus);
  }

  override onKeyDown(
    ev: KeyDownEvent<GpuMemoryUsageSettings>
  ): Promise<void> | void {
    let gpu = this.getGpu(ev.payload.settings.gpuId);
    gpu?.launchAssociatedApp();
  }

  override onWillAppear(
    ev: WillAppearEvent<GpuMemoryUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    this.startTimer(gpu!, ev.action, ev.payload.settings);
  }

  override onWillDisappear(
    ev: WillDisappearEvent<GpuMemoryUsageSettings>
  ): void {
    clearInterval(this.timers.get(ev.action.id));
  }
}

type GpuMemoryUsageSettings = {
  gpuId: string;
  showAsPercentage: string;
};
