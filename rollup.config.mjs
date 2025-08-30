import commonjs from "@rollup/plugin-commonjs";
import json from "@rollup/plugin-json";
import nodeResolve from "@rollup/plugin-node-resolve";
import terser from "@rollup/plugin-terser";
import typescript from "@rollup/plugin-typescript";
import path from "node:path";
import url from "node:url";
import copy from "rollup-plugin-copy";
import nativePlugin from "rollup-plugin-natives";
import replace from "@rollup/plugin-replace";
import { name } from "happy-dom/cjs/PropertySymbol.cjs";

const isWatching = !!process.env.ROLLUP_WATCH;
const sdPlugin = "com.nthompson.gpu.sdPlugin";

/**
 * @type {import('rollup').RollupOptions}
 */
const config = {
  input: "src/plugin.ts",
  output: {
    file: `${sdPlugin}/bin/plugin.js`,
    sourcemap: isWatching,
    sourcemapPathTransform: (relativeSourcePath, sourcemapPath) => {
      return url.pathToFileURL(
        path.resolve(path.dirname(sourcemapPath), relativeSourcePath)
      ).href;
    },
    format: "commonjs",
  },
  external: "@thompsonnoahe/macos-metrics",
  plugins: [
    replace({
      include: "node_modules\\@thompsonnoahe\\macos-metrics\\*.js",
      values: {
        "require = ": "myRequire = ",
      },
      preventAssignment: false,
    }),
    {
      name: "watch-externals",
      buildStart: function () {
        this.addWatchFile(`${sdPlugin}/manifest.json`);
      },
    },
    typescript({
      mapRoot: isWatching ? "./" : undefined,
    }),
    json(),
    nodeResolve({
      browser: false,
      exportConditions: ["node"],
      preferBuiltins: true,
    }),
    commonjs(),
    !isWatching && terser(),
    {
      name: "emit-module-package-file",
      generateBundle() {
        this.emitFile({
          fileName: "package.json",
          source: `{ "type": "common" }`,
          type: "asset",
        });
      },
    },
    copy({
      copyOnce: true,
      errorOnExist: false,
      overwrite: true,
      targets: [
        {
          src: "build/Release/gpu.node",
          dest: `${sdPlugin}/bin/`,
        },
        {
          src: "node_modules/@thompsonnoahe/macos-metrics-darwin-arm64/macos_metrics.darwin-arm64.node",
          dest: `${sdPlugin}/bin/`,
        },
      ],
    }),
  ],
};

export default config;
