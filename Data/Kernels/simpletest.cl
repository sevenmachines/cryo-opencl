__kernel void hello(__global float *input1 ,__global float *input2, __global float *output) {
	size_t id = get_global_id(0);
	output[id] = input1[id] * input2[id];
	}