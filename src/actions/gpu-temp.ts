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

@action({ UUID: 'com.nthompson.gpu-metrics.temp' })
export class GpuTemp extends SingletonAction<GpuTempSettings> {
  timers: Map<string, NodeJS.Timeout> = new Map();
  query = query;
  devices = this.query.getGpus();

  getGpu(gpuId: string): Gpu | undefined {
    return this.devices.find((gpu: Gpu) => gpu.deviceId === gpuId);
  }

  startTimer(gpu: Gpu, action: any, celsius: boolean = true): void {
    if (this.timers.has(action.id)) {
      clearInterval(this.timers.get(action.id)!);
    }

    this.timers.set(
      action.id,
      setInterval(() => {
        if (gpu === undefined) {
          streamDeck.logger.error('GPU not found or selected');
          return;
        }

        let temp = gpu?.temperature;

        if (temp === -1) {
          action.showAlert();
          return;
        }

        if (!celsius) {
          temp = (temp * 9) / 5 + 32;
        }

        action.setTitle(`${temp}°${celsius ? 'C' : 'F'}`);
      }, 1000)
    );
  }

  override onDidReceiveSettings(
    ev: DidReceiveSettingsEvent<GpuTempSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    let celsius = true;

    if (ev.payload.settings.temp === 'fahrenheit') {
      celsius = false;
    }

    this.startTimer(gpu!, ev.action, celsius);
  }

  override onPropertyInspectorDidAppear(
    ev: PropertyInspectorDidAppearEvent<GpuTempSettings>
  ): Promise<void> | void {
    const gpus = this.devices.map((gpu: Gpu) => {
      return {
        title: "#" + gpu.index + " " + gpu.name,
        value: gpu.deviceId,
      };
    });

    streamDeck.ui.current?.sendToPropertyInspector(gpus);
  }

  override onKeyDown(ev: KeyDownEvent<GpuTempSettings>): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);
    gpu?.launchAssociatedApp();
  }

  override onWillAppear(
    ev: WillAppearEvent<GpuTempSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    let celsius = true;

    if (ev.payload.settings.temp === 'fahrenheit') {
      celsius = false;
    }

    this.startTimer(gpu!, ev.action, celsius);
  }

  override onWillDisappear(ev: WillDisappearEvent<GpuTempSettings>): void {
    clearInterval(this.timers.get(ev.action.id));
  }
}

type GpuTempSettings = {
  gpuId: string;
  temp: string;
};
