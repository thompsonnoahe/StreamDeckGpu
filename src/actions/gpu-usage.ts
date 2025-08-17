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
import * as d3 from 'd3';
import { Window } from 'happy-dom';
import Buffer from '../utils/buffer';

const width = 25;
const height = 25;

@action({ UUID: 'com.nthompson.gpu.usage' })
export class GpuUsage extends SingletonAction<GpuUsageSettings> {
  timers: Map<string, NodeJS.Timeout | undefined> = new Map();
  query = query;
  devices = this.query.getGpus();
  buff = new Buffer<[number, number]>(width);
  window = new Window();

  getGpu(gpuId: string): Gpu | undefined {
    return this.devices.find((gpu: Gpu) => gpu.deviceId === gpuId);
  }

  createChart(
    svg: d3.Selection<any, any, any, any>,
    gpu: Gpu,
    settings: GpuUsageSettings
  ) {
    const x = d3
      .scaleTime()
      .domain([new Date(new Date().getTime() - width * 1000), new Date()])
      .range([0, width]);
    const y = d3.scaleLinear([0, 100], [height, 0]);

    this.buff.enqueue([Date.now(), gpu?.usage]);

    const area = d3
      .area()
      .x(d => x(d[0]))
      .y0(height)
      .y1(d => y(d[1]));

    svg.selectAll('path').remove();

    svg
      .append('path')
      .attr('d', area(this.buff.buffer))
      .attr('fill', settings.chartColor);

    return svg;
  }

  startTimer(gpu: Gpu, action: any, settings: GpuUsageSettings): void {
    if (this.timers.has(action.id)) {
      clearInterval(this.timers.get(action.id));
    }

    const svg = d3
      .select(this.window.document.body)
      .append('svg')
      .attr('width', width)
      .attr('height', height);

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

        if (settings.enableChart) {
          const chart = this.createChart(svg, gpu, settings);

          action.setImage(
            `data:image/svg+xml;charset=utf8,${encodeURIComponent(chart.node())}`
          );
        } else {
          action.setImage('gpu.png');
        }
      }, 1000)
    );
  }

  override onDidReceiveSettings(
    ev: DidReceiveSettingsEvent<GpuUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    this.startTimer(gpu!, ev.action, ev.payload.settings);
  }

  override onPropertyInspectorDidAppear(
    ev: PropertyInspectorDidAppearEvent<GpuUsageSettings>
  ): Promise<void> | void {
    const gpus = this.devices.map((gpu: Gpu) => {
      return {
        title: gpu.name,
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
    this.startTimer(gpu!, ev.action, ev.payload.settings);
  }

  override onWillDisappear(ev: WillDisappearEvent<GpuUsageSettings>): void {
    clearInterval(this.timers.get(ev.action.id));
  }
}

type GpuUsageSettings = {
  gpuId: string;
  chartColor: string;
  enableChart: boolean;
};
