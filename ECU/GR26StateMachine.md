# ECU State Machine Diagram for GR26

## Key
- **Green:** Standard start up
- **Blue:** Shutdown without critical errors
- **Red:** Critical error handling
- **Pink:** Between drive active substates

```mermaid
%%{init: {'theme':'base', 'themeVariables': {
    'primaryColor': '#d1d1f6',
    'primaryBorderColor': '#4e2cb0',
    'primaryTextColor': '#000',
    'edgeLabelBackground': '#f1e9dc',
    'fontSize': '24px'
}}}%%

flowchart TD;
    A(GLV Off) -->|GLVMS On| B(GLV On);
    linkStyle 0 stroke:#4CBB17,stroke-width:4px;

    B -->|GLVMS Off| A;
    linkStyle 1 stroke:#0096FF,stroke-width:4px;

    B -->|TS Active On| C(Precharge Engaged);
    linkStyle 2 stroke:#4CBB17,stroke-width:4px;

    C --->|"TS Active Off"| B;
    linkStyle 3 stroke:#0096FF,stroke-width:4px;

    C -->|ACU Precharge Confirmation| D(Precharging);
    linkStyle 4 stroke:#4CBB17,stroke-width:4px;

    D --> |Precharge Success Confirmation| E(Precharge Complete);
    linkStyle 5 stroke:#4CBB17,stroke-width:4px;

    E --> |Brake + RTD on| F(Drive Standby);
    linkStyle 6 stroke:#4CBB17,stroke-width:4px;

    F --> |RTD Off| E;
    linkStyle 7 stroke:#0096FF,stroke-width:4px;

    F --> |Valid Torque Request| G(Drive Active);
    linkStyle 8 stroke:#4CBB17,stroke-width:4px;

    G ---> |Non-critical Violation| F;
    linkStyle 9 stroke:#E7180B,stroke-width:4px;

    G --> |RTD Off| F;
    linkStyle 10 stroke:#0096FF,stroke-width:4px;

    G -..- Group1;
    linkStyle 11 stroke:#fc8eac,stroke-width:5px;

    subgraph Group1 [SUBSTATES]
        H([Idle]) <--> I(["Power"]) <--> J([Regen]) <--> H
        linkStyle 12 stroke:#fc8eac,stroke-width:3px;
        linkStyle 13 stroke:#fc8eac,stroke-width:3px;
        linkStyle 14 stroke:#fc8eac,stroke-width:3px;

        Note["*Throttle map dependent*"]:::note
    end

    H --> |Inactivity Frame Filter| F;
    linkStyle 15 stroke:#0096FF,stroke-width:4px;

    %% Connections to TS discharge off
    D --> |TS Active Off| TS(TS Discharge Off);
    linkStyle 16 stroke:#0096FF,stroke-width:4px;

    D --> |Precharge Cancellation| TS;
    linkStyle 17 stroke:#0096FF,stroke-width:4px;

    E --> |TS Active Off / ACU Shutdown| TS;
    linkStyle 18 stroke:#0096FF,stroke-width:4px;

    F --> |TS Active Off / ACU Shutdown| TS;
    linkStyle 19 stroke:#0096FF,stroke-width:4px;

    G ---> |TS Active Off / ACU Shutdown| TS;
    linkStyle 20 stroke:#0096FF,stroke-width:4px;

    %% Error state
    B --> |"Critical Communication Error"| L["**ERROR**"];
    linkStyle 21 stroke:#E7180B,stroke-width:4px;

    C --> |"Critical Communication Error"| L;
    linkStyle 22 stroke:#E7180B,stroke-width:4px;

    D --> |Critical Error| L;
    linkStyle 23 stroke:#E7180B,stroke-width:4px;

    E --> |Critical Error| L;
    linkStyle 24 stroke:#E7180B,stroke-width:4px;

    F --> |Critical Error| L;
    linkStyle 25 stroke:#E7180B,stroke-width:4px;

    G --> |Critical Violation| L;
    linkStyle 26 stroke:#E7180B,stroke-width:4px;

    TS ---> |Unresolved Errors| L
    linkStyle 27 stroke:#E7180B,stroke-width:4px;

    L --> |TSV > 60V| TS
    linkStyle 28 stroke:#E7180B,stroke-width:4px;

    L --> |Errors Resolved| B
    linkStyle 29 stroke:#4CBB17,stroke-width:4px;

    TS --> |TSV < 60V + Timeout Reached| L;
    linkStyle 30 stroke:#E7180B,stroke-width:4px;

    TS --> |TSV < 60V + Errors Resolved| B;
    linkStyle 31 stroke:#4CBB17,stroke-width:4px;

    %% Extra styling
    style L fill:#F0C6C6, stroke:#5D1818;

    classDef sg fill:#FAFAD2, stroke:#121a23,font-weight:bold;
    class Group1 sg;

    classDef note fill:none,stroke:none,color:#000,font-style:italic;
```
