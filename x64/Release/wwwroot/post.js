//html发送post请求功能模块
function RZHttpRequest(url, timeout, mode, postdata, callback) {
    var request = new XMLHttpRequest();
    var r_timeout = false;
    var timer = setTimeout(function () {
        r_timeout = true;
        request.abort();
    }, timeout);

    switch (mode) {
        case 'POST':
            request.open('POST', url, 1);
            break;
    }

    request.onreadystatechange = function () {
        if (request.readyState !== 4) return;
        if (r_timeout) {
            callback(3, null)
            return;
        }
        clearTimeout(timer);
        if (request.status === 304 || (request.status >= 200 && request.status < 300)) {
            callback(1, request.responseText)
        } else {
            callback(2, request.status)
        }
    }
    switch (mode) {
        case 'GET':
            request.send(null);
            break;
        case 'POST':
            request.send(postdata);
            break;
    }
}

