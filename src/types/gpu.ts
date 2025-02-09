import Vendor from './vendor';
type Gpu = {
  vendor: Vendor;
  name: string;
  index: number;
  deviceId: string;
  usage: number;
  temperature: number;
  memory: number;
  usedMemory: number;
  launchAssociatedApp: () => void;
};

export { Gpu };
