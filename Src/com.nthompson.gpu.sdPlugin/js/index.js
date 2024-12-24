const select = document.getElementById('gpuSelect');

select.addEventListener('change', e => {
    sendValueToPlugin(e.target.value, 'gpuInfo');
})

let socket;
let context;
let uuid;
let info;

window.connectElgatoStreamDeckSocket = (inPort, inPropertyInspectorUUID, inRegisterEvent, inInfo, inActionInfo) => {
    info = JSON.parse(inActionInfo);
    uuid = inPropertyInspectorUUID;
    socket = new WebSocket('ws://127.0.0.1:' + inPort);

    console.log('Connected to Elgato.');

    socket.addEventListener('open', () => {
        const json = {
            event: inRegisterEvent,
            uuid,
        }
        socket.send(JSON.stringify(json));

        const message = {
            action: info['action'],
            event: 'sendToPlugin',
            context: uuid,
            payload: {
                propertyInspectorLoaded: true,
                receiveSelection: false
            },
        }
        socket.send(JSON.stringify(message));
    });

    socket.addEventListener('message', (e) => {
        const info = JSON.parse(e.data);

        console.log(info);

        const payload = info['payload'];

        if (!payload) return;

        if (payload["gpus"]) {
            const gpus = payload['gpus'];
            const selection = payload['selected'];

            for (let i = 0; i < gpus?.length; i++) {
                select.add(new Option(gpus[i][1].name, JSON.stringify(gpus[i][1]), false, gpus[i][1].deviceId === selection['deviceId']));
            }

            if (gpus.length === 0) {
                select.add(new Option("No GPUs found", null, false, true));
            }

            if (gpus.length === 1) {
                sendValueToPlugin(JSON.stringify(gpus[0][1]), 'gpuInfo');
            }
        }

        if (payload["settings"]) {
            const gpuInfo = payload["settings"]["gpuInfo"]
            const options = [...select.options];
            select.selectedIndex = options.findIndex(o => JSON.parse(o.value).deviceId === gpuInfo["deviceId"]);
        }

        if (!payload["gpus"] && !payload["settings"]) {
            select.add(new Option("No GPUs found", null, false, true));
        }
    })

    socket.addEventListener('close', () => {
        console.log('Connection closed');
    })
}


const sendValueToPlugin = (data, param) => {
    const json = {
        action: info['action'],
        event: 'sendToPlugin',
        context: uuid,
        payload: {
            receiveSelection: true,
            propertyInspectorLoaded: false,
            [param]: JSON.parse(data)
        },
    }

    socket.send(JSON.stringify(json));
}

