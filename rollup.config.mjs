import commonjs from "@rollup/plugin-commonjs";
import json from "@rollup/plugin-json";
import nodeResolve from "@rollup/plugin-node-resolve";
import terser from "@rollup/plugin-terser";
import typescript from "@rollup/plugin-typescript";
import path from "node:path";
import url from "node:url";
import copy from "rollup-plugin-copy";

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
    inlineDynamicImports: true,
    format: "commonjs",
  },
  external: "@thompsonnoahe/macos-metrics-darwin-arm64",
  plugins: [
    {
      name: "watch-externals",
      buildStart: function () {
        this.addWatchFile(`${sdPlugin}/manifest.json`);
      },
    },
    commonjs(),
    typescript({
      mapRoot: isWatching ? "./" : undefined,
    }),
    json(),
    nodeResolve({
      browser: false,
      exportConditions: ["node"],
      preferBuiltins: true,
    }),
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
          src: "node_modules/@thompsonnoahe/macos-metrics/build/Release/macos_metrics.darwin-arm64.node",
          dest: `${sdPlugin}/bin/`,
        },
      ],
    }),
  ],
};

export default config;
