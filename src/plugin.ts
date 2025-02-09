import streamDeck, { LogLevel } from '@elgato/streamdeck';

import { GpuUsage } from './actions/gpu-usage';
import { GpuTemp } from './actions/gpu-temp';
import { GpuMemoryUsage } from './actions/gpu-mem';

// We can enable "trace" logging so that all messages between the Stream Deck, and the plugin are recorded. When storing sensitive information
streamDeck.logger.setLevel(LogLevel.TRACE);

// Register actions
streamDeck.actions.registerAction(new GpuUsage());
streamDeck.actions.registerAction(new GpuTemp());
streamDeck.actions.registerAction(new GpuMemoryUsage());

// Finally, connect to the Stream Deck.
streamDeck.connect();
