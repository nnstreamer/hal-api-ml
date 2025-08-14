# HAL ML API Overview

## Introduction

The HAL ML API provides an interface for [Tizen's higher-level Machine Learning API](https://docs.tizen.org/application/native/guides/machine-learning/overview) to interact with various hardware acceleration devices (e.g., NPUs, GPUs, DSPs) via [HAL ML Accelerator Backends](https://github.com/nnstreamer/hal-backend-ml-accelerator). **Users would not use this directly**, it is used in implementation of [NNStreamer](https://github.com/nnstreamer/nnstreamer)'s `tensor_filter` sub-plugin which is backend of Tizen ML API.

## Architecture

```mermaid
graph TD
    subgraph TF["Tizen Framework"]
        A["Tizen ML API"]
    end

    subgraph HM["HAL API ML"]
        D["Tizen HAL API for ML"]
    end

    subgraph HBMA["HAL Backend ML Accelerator"]
        E["backend for each vendor"]
    end

    subgraph MA["Device"]
        CPU
        GPU
        NPU
        DSP
    end

    TF <--> HM <--> HBMA <--> MA

    style TF fill:#68b,color:#000,stroke:#000
    style HM fill:#68b,color:#000,stroke:#000
    style HBMA fill:#68b,color:#000,stroke:#000
    style MA fill:#68b,color:#000,stroke:#000

    style A fill:#fff,color:#000,stroke:#000
    style D fill:#fff,color:#000,stroke:#000
    style E fill:#fff,color:#000,stroke:#000
    style CPU fill:#fff,color:#000,stroke:#000
    style GPU fill:#fff,color:#000,stroke:#000
    style NPU fill:#fff,color:#000,stroke:#000
    style DSP fill:#fff,color:#000,stroke:#000
```

## License

This project is licensed under the Apache License, Version 2.0. See the [LICENSE](LICENSE) file for details.
