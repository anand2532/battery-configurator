#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    double capacity;    // mAh
    double voltage;     // V
    double maxCurrent;  // A
    double weight;      // g
    double length;      // mm
    double width;       // mm
    double height;      // mm
} CellSpecs;

typedef struct {
    int seriesCount;
    int parallelCount;
    double totalVoltage;
    double totalCapacity;
    double totalWeight;
    double totalPower;
    double maxCurrent;
    double volumeWidth;
    double volumeLength;
    double volumeHeight;
} BatteryConfig;

// Calculate power from voltage and capacity
double calculatePower(double voltage, double capacity) {
    return voltage * (capacity / 1000.0);  // Convert mAh to Ah
}

// Generate possible configurations based on target power
BatteryConfig* generateConfigurations(CellSpecs cell, double targetPower, 
                                    int maxSeries, int maxParallel, int* configCount) {
    BatteryConfig* configs = malloc(sizeof(BatteryConfig) * maxSeries * maxParallel);
    *configCount = 0;

    for (int s = 1; s <= maxSeries; s++) {
        for (int p = 1; p <= maxParallel; p++) {
            double totalVoltage = cell.voltage * s;
            double totalCapacity = cell.capacity * p;
            double power = calculatePower(totalVoltage, totalCapacity);
            
            // Check if configuration meets power requirements within 10% tolerance
            if (fabs(power - targetPower) <= targetPower * 0.1) {
                BatteryConfig config = {
                    .seriesCount = s,
                    .parallelCount = p,
                    .totalVoltage = totalVoltage,
                    .totalCapacity = totalCapacity,
                    .totalWeight = cell.weight * s * p,
                    .totalPower = power,
                    .maxCurrent = cell.maxCurrent * p,
                    .volumeWidth = cell.width * p,
                    .volumeLength = cell.length,
                    .volumeHeight = cell.height * s
                };
                configs[*configCount] = config;
                (*configCount)++;
            }
        }
    }
    return configs;
}

// Score configurations based on various factors
double scoreConfiguration(BatteryConfig config, double targetPower) {
    double powerScore = 1.0 - (fabs(config.totalPower - targetPower) / targetPower);
    double weightScore = 1.0 / (config.totalWeight / 1000.0);  // Convert to kg
    double volumeScore = 1.0 / (config.volumeWidth * config.volumeLength * config.volumeHeight / 1000000.0);  // Convert to cm³
    
    // Weighted scoring (adjust weights based on priorities)
    return powerScore * 0.5 + weightScore * 0.3 + volumeScore * 0.2;
}

// Find best configuration based on scoring
BatteryConfig findBestConfiguration(BatteryConfig* configs, int configCount, double targetPower) {
    double bestScore = -1;
    int bestIndex = 0;
    
    for (int i = 0; i < configCount; i++) {
        double score = scoreConfiguration(configs[i], targetPower);
        if (score > bestScore) {
            bestScore = score;
            bestIndex = i;
        }
    }
    
    return configs[bestIndex];
}

// Main function to process input and generate output
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    // Read input from file
    FILE *input = fopen(argv[1], "r");
    if (!input) {
        printf("Error opening input file\n");
        return 1;
    }

    double targetPower;
    CellSpecs cell;
    fscanf(input, "%lf", &targetPower);
    fscanf(input, "%lf", &cell.capacity);
    fscanf(input, "%lf", &cell.voltage);
    fscanf(input, "%lf", &cell.maxCurrent);
    fscanf(input, "%lf", &cell.weight);
    fscanf(input, "%lf", &cell.length);
    fscanf(input, "%lf", &cell.width);
    fscanf(input, "%lf", &cell.height);
    fclose(input);

    // Generate and find best configuration
    int configCount;
    BatteryConfig* configs = generateConfigurations(cell, targetPower, 10, 10, &configCount);
    if (configCount == 0) {
        printf("No valid configurations found\n");
        free(configs);
        return 1;
    }

    BatteryConfig bestConfig = findBestConfiguration(configs, configCount, targetPower);

    // Write output to file
    FILE *output = fopen(argv[2], "w");
    if (!output) {
        printf("Error opening output file\n");
        free(configs);
        return 1;
    }

    fprintf(output, "Best Configuration:\n");
    fprintf(output, "Series: %d\n", bestConfig.seriesCount);
    fprintf(output, "Parallel: %d\n", bestConfig.parallelCount);
    fprintf(output, "Total Voltage: %.2f V\n", bestConfig.totalVoltage);
    fprintf(output, "Total Capacity: %.2f mAh\n", bestConfig.totalCapacity);
    fprintf(output, "Total Power: %.2f Wh\n", bestConfig.totalPower);
    fprintf(output, "Total Weight: %.2f g\n", bestConfig.totalWeight);
    fprintf(output, "Max Current: %.2f A\n", bestConfig.maxCurrent);
    fprintf(output, "Volume: %.2f x %.2f x %.2f mm\n", 
            bestConfig.volumeWidth, bestConfig.volumeLength, bestConfig.volumeHeight);

    fclose(output);
    free(configs);
    return 0;
}