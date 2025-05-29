import streamDeck, {
  action,
  DidReceiveSettingsEvent,
  KeyDownEvent,
  PropertyInspectorDidAppearEvent,
  SingletonAction,
  WillAppearEvent,
  WillDisappearEvent,
} from '@elgato/streamdeck';
import query from '../query';
import { Gpu } from '../types/gpu';
import Vendor from '../types/vendor';

@action({ UUID: 'com.nthompson.gpu.power' })
export class GpuPowerUsage extends SingletonAction<GpuPowerUsageSettings> {
  timers: Map<string, NodeJS.Timeout> = new Map();
  query = query;
  devices = this.query.getGpus();

  getGpu(gpuId: string): Gpu | undefined {
    return this.devices.find((gpu: Gpu) => gpu.deviceId === gpuId);
  }

  startTimer(gpu: Gpu, action: any) {
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

        if (gpu.power === -1) {
          action.showAlert();
          return;
        }

        let power = gpu.power;

        if (gpu.vendor === Vendor.Nvidia) {
          // NVIDIA returns mW, convert to W
          power /= 1000;
        }

        action.setTitle(`${Math.round(power)}W`);
      }, 1000)
    );
  }

  override onDidReceiveSettings(
    ev: DidReceiveSettingsEvent<GpuPowerUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    this.startTimer(gpu!, ev.action);
  }

  override onKeyDown(
    ev: KeyDownEvent<GpuPowerUsageSettings>
  ): Promise<void> | void {
    let gpu = this.getGpu(ev.payload.settings.gpuId);
    gpu?.launchAssociatedApp();
  }

  override onPropertyInspectorDidAppear(
    ev: PropertyInspectorDidAppearEvent<GpuPowerUsageSettings>
  ): Promise<void> | void {
    const gpus = this.devices.map((gpu: Gpu) => {
      return {
        title: gpu.name,
        value: gpu.deviceId,
      };
    });

    streamDeck.ui.current?.sendToPropertyInspector(gpus);
  }

  override onWillAppear(
    ev: WillAppearEvent<GpuPowerUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    this.startTimer(gpu!, ev.action);
  }

  override onWillDisappear(
    ev: WillDisappearEvent<GpuPowerUsageSettings>
  ): void {
    clearInterval(this.timers.get(ev.action.id));
  }
}

type GpuPowerUsageSettings = {
  gpuId: string;
};
