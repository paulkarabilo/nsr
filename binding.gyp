{
    "targets": [{
        "target_name": "napi-experiment",
        'include_dirs': ['./include'],
        "sources": [ "src/addon_node.c", "src/addon.c", "src/srv.c", "src/callbacks.c" ],
        "cflags": [
            "-std=c99",
            "-D_GNU_SOURCE"
        ]
    }]
}