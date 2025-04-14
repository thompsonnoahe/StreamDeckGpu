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

@action({ UUID: 'com.nthompson.gpu-metrics.usage' })
export class GpuUsage extends SingletonAction<GpuUsageSettings> {
  timers: Map<string, NodeJS.Timeout | undefined> = new Map();
  query = query;
  devices = this.query.getGpus();

  getGpu(gpuId: string): Gpu | undefined {
    return this.devices.find((gpu: Gpu) => gpu.deviceId === gpuId);
  }

  startTimer(gpu: Gpu, action: any): void {
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

        if (gpu?.usage === -1) {
          action.showAlert();
          return;
        }

        action.setTitle(`${gpu?.usage}%`);
      }, 1000)
    );
  }

  override onDidReceiveSettings(
    ev: DidReceiveSettingsEvent<GpuUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    this.startTimer(gpu!, ev.action);
  }

  override onPropertyInspectorDidAppear(
    ev: PropertyInspectorDidAppearEvent<GpuUsageSettings>
  ): Promise<void> | void {
    const gpus = this.devices.map((gpu: Gpu) => {
      return {
        title: "#" + gpu.index + " " + gpu.name,
        value: gpu.deviceId,
      };
    });

    streamDeck.ui.current?.sendToPropertyInspector(gpus);
  }

  override onKeyDown(ev: KeyDownEvent<GpuUsageSettings>): Promise<void> | void {
    let gpu = this.getGpu(ev.payload.settings.gpuId);
    gpu?.launchAssociatedApp();
  }

  override onWillAppear(
    ev: WillAppearEvent<GpuUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);
    this.startTimer(gpu!, ev.action);
  }

  override onWillDisappear(ev: WillDisappearEvent<GpuUsageSettings>): void {
    clearInterval(this.timers.get(ev.action.id));
  }
}

type GpuUsageSettings = {
  gpuId: string;
};
