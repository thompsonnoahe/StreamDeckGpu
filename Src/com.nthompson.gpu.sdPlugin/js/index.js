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

        const payload = info['payload'];

        if (!payload) return;

        const gpus = payload['gpus'];
        const selected = payload['selected'];

        for (let i = 0; i < gpus?.length; i++) {
            select.add(new Option(gpus[i][1].name, JSON.stringify(gpus[i][1]), false, selected > 0 ? i === selected : i === 0));
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

