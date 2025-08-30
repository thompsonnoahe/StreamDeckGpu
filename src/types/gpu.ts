import Vendor from "./vendor";
type Gpu = {
  vendor: Vendor;
  name: string;
  index: number;
  deviceId: string;
  usage: number;
  temperature: number;
  memory: number | undefined;
  usedMemory: number | undefined;
  power: number;
  launchAssociatedApp: () => void;
};

export { Gpu };
