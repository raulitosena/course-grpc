# Protocol Documentation
<a name="top"></a>

## Table of Contents

- [average.proto](#average-proto)
    - [AvgSample](#average-AvgSample)
    - [AvgTotal](#average-AvgTotal)
  
    - [AverageService](#average-AverageService)
  
- [checksum.proto](#checksum-proto)
    - [FileUploadRequest](#checksum-FileUploadRequest)
    - [FileUploadResponse](#checksum-FileUploadResponse)
  
    - [FileChecksumService](#checksum-FileChecksumService)
  
- [evens.proto](#evens-proto)
    - [Number](#evens-Number)
  
    - [EvensService](#evens-EvensService)
  
- [fibonacci.proto](#fibonacci-proto)
    - [FibonacciListResponse](#fibonacci-FibonacciListResponse)
    - [FibonacciRequest](#fibonacci-FibonacciRequest)
    - [FibonacciResponse](#fibonacci-FibonacciResponse)
  
    - [FibonacciService](#fibonacci-FibonacciService)
    - [FibonacciSlowService](#fibonacci-FibonacciSlowService)
  
- [health.proto](#health-proto)
    - [HealthCheckRequest](#grpc-health-v1-HealthCheckRequest)
    - [HealthCheckResponse](#grpc-health-v1-HealthCheckResponse)
  
    - [HealthCheckResponse.ServingStatus](#grpc-health-v1-HealthCheckResponse-ServingStatus)
  
    - [Health](#grpc-health-v1-Health)
  
- [math.proto](#math-proto)
    - [OperandRequest](#math-OperandRequest)
    - [ResultResponse](#math-ResultResponse)
    - [SqrtRequest](#math-SqrtRequest)
    - [SqrtResponse](#math-SqrtResponse)
  
    - [MathService](#math-MathService)
    - [SqrtService](#math-SqrtService)
  
- [sum.proto](#sum-proto)
    - [SumOperand](#sum-SumOperand)
    - [SumResult](#sum-SumResult)
  
    - [SumService](#sum-SumService)
  
- [Scalar Value Types](#scalar-value-types)



<a name="average-proto"></a>
<p align="right"><a href="#top">Top</a></p>

## average.proto



<a name="average-AvgSample"></a>

### AvgSample
Message representing a sample value for calculating an average.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| value | [int32](#int32) |  | [required] A sample value to be used in the average calculation. |






<a name="average-AvgTotal"></a>

### AvgTotal
Message representing the final average result.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| value | [float](#float) |  | [required] The computed average value. |





 

 

 


<a name="average-AverageService"></a>

### AverageService
Service that computes the average of a stream of values.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| ComputeAvg | [AvgSample](#average-AvgSample) stream | [AvgTotal](#average-AvgTotal) | RPC method that computes the average of a stream of sample values. The client sends a stream of AvgSample, and the server returns the final AvgTotal. |

 



<a name="checksum-proto"></a>
<p align="right"><a href="#top">Top</a></p>

## checksum.proto



<a name="checksum-FileUploadRequest"></a>

### FileUploadRequest
Message containing the file data for upload.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| file_data | [bytes](#bytes) |  | [required] The file data to be uploaded as bytes. |






<a name="checksum-FileUploadResponse"></a>

### FileUploadResponse
Message containing the computed checksum of the uploaded file.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| checksum | [string](#string) |  | [required] The computed checksum of the uploaded file as a string. |





 

 

 


<a name="checksum-FileChecksumService"></a>

### FileChecksumService
Service that handles file uploads and computes file checksums.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| UploadFile | [FileUploadRequest](#checksum-FileUploadRequest) | [FileUploadResponse](#checksum-FileUploadResponse) | RPC method for uploading a file. The client sends a FileUploadRequest, and the server returns the checksum in a FileUploadResponse. |

 



<a name="evens-proto"></a>
<p align="right"><a href="#top">Top</a></p>

## evens.proto



<a name="evens-Number"></a>

### Number
Message representing a number to check for evenness.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| value | [int32](#int32) |  | [required] A number to be checked for evenness. |





 

 

 


<a name="evens-EvensService"></a>

### EvensService
Service that streams even numbers based on input numbers.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| FindEvens | [Number](#evens-Number) stream | [Number](#evens-Number) stream | RPC method that streams even numbers. The client sends a stream of Number messages, and the server returns a stream of even Number messages. |

 



<a name="fibonacci-proto"></a>
<p align="right"><a href="#top">Top</a></p>

## fibonacci.proto



<a name="fibonacci-FibonacciListResponse"></a>

### FibonacciListResponse
Message for a response containing a list of Fibonacci numbers.
This holds a list of all calculated Fibonacci numbers for a given request.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| number | [uint64](#uint64) | repeated | [required] A list of Fibonacci numbers. |






<a name="fibonacci-FibonacciRequest"></a>

### FibonacciRequest
Message for a Fibonacci request.
This contains the number for which Fibonacci numbers will be calculated.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| number | [uint64](#uint64) |  | [required] The number for which to calculate Fibonacci. |






<a name="fibonacci-FibonacciResponse"></a>

### FibonacciResponse
Message for a Fibonacci response.
This contains a Fibonacci number in the sequence.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| number | [uint64](#uint64) |  | [required] A Fibonacci number in the sequence. |





 

 

 


<a name="fibonacci-FibonacciService"></a>

### FibonacciService
Service that provides Fibonacci number streaming.
It sends a stream of Fibonacci numbers for a given request.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| GetFibonacciStream | [FibonacciRequest](#fibonacci-FibonacciRequest) | [FibonacciResponse](#fibonacci-FibonacciResponse) stream | RPC method that streams Fibonacci numbers in response to a request. The client sends a FibonacciRequest containing a number, and the server returns a stream of FibonacciResponse messages, each containing one Fibonacci number. |


<a name="fibonacci-FibonacciSlowService"></a>

### FibonacciSlowService
Service that provides a list of Fibonacci numbers.
It returns all Fibonacci numbers up to the requested number at once.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| GetFibonacciList | [FibonacciRequest](#fibonacci-FibonacciRequest) | [FibonacciListResponse](#fibonacci-FibonacciListResponse) | RPC method that returns a list of Fibonacci numbers for a given request. The client sends a FibonacciRequest containing a number, and the server returns a FibonacciListResponse containing all Fibonacci numbers up to that number. |

 



<a name="health-proto"></a>
<p align="right"><a href="#top">Top</a></p>

## health.proto



<a name="grpc-health-v1-HealthCheckRequest"></a>

### HealthCheckRequest



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| service | [string](#string) |  |  |






<a name="grpc-health-v1-HealthCheckResponse"></a>

### HealthCheckResponse



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| status | [HealthCheckResponse.ServingStatus](#grpc-health-v1-HealthCheckResponse-ServingStatus) |  |  |





 


<a name="grpc-health-v1-HealthCheckResponse-ServingStatus"></a>

### HealthCheckResponse.ServingStatus


| Name | Number | Description |
| ---- | ------ | ----------- |
| UNKNOWN | 0 |  |
| SERVING | 1 |  |
| NOT_SERVING | 2 |  |
| SERVICE_UNKNOWN | 3 | Used only by the Watch method. |


 

 


<a name="grpc-health-v1-Health"></a>

### Health


| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| Check | [HealthCheckRequest](#grpc-health-v1-HealthCheckRequest) | [HealthCheckResponse](#grpc-health-v1-HealthCheckResponse) | If the requested service is unknown, the call will fail with status NOT_FOUND. |
| Watch | [HealthCheckRequest](#grpc-health-v1-HealthCheckRequest) | [HealthCheckResponse](#grpc-health-v1-HealthCheckResponse) stream | Performs a watch for the serving status of the requested service. The server will immediately send back a message indicating the current serving status. It will then subsequently send a new message whenever the service&#39;s serving status changes.

If the requested service is unknown when the call is received, the server will send a message setting the serving status to SERVICE_UNKNOWN but will *not* terminate the call. If at some future point, the serving status of the service becomes known, the server will send a new message with the service&#39;s serving status.

If the call terminates with status UNIMPLEMENTED, then clients should assume this method is not supported and should not retry the call. If the call terminates with any other status (including OK), clients should retry the call with appropriate exponential backoff. |

 



<a name="math-proto"></a>
<p align="right"><a href="#top">Top</a></p>

## math.proto



<a name="math-OperandRequest"></a>

### OperandRequest
Message representing a number for multiplication.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| number | [int32](#int32) |  | [required] The number to be multiplied. |






<a name="math-ResultResponse"></a>

### ResultResponse
Message representing the result of a multiplication.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| result | [int32](#int32) |  | [required] The result of the multiplication. |






<a name="math-SqrtRequest"></a>

### SqrtRequest
Message representing a number for square root calculation.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| number | [int32](#int32) |  | [required] The number for which the square root is to be calculated. |






<a name="math-SqrtResponse"></a>

### SqrtResponse
Message representing the square root result.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| result | [double](#double) |  | [required] The square root result as a double. |





 

 

 


<a name="math-MathService"></a>

### MathService
Service that provides various multiplication operations.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| Calculate2x | [OperandRequest](#math-OperandRequest) | [ResultResponse](#math-ResultResponse) | RPC method to multiply the input by 2. |
| Calculate3x | [OperandRequest](#math-OperandRequest) | [ResultResponse](#math-ResultResponse) | RPC method to multiply the input by 3. |
| Calculate4x | [OperandRequest](#math-OperandRequest) | [ResultResponse](#math-ResultResponse) | RPC method to multiply the input by 4. |
| Calculate5x | [OperandRequest](#math-OperandRequest) | [ResultResponse](#math-ResultResponse) | RPC method to multiply the input by 5. |
| Calculate6x | [OperandRequest](#math-OperandRequest) | [ResultResponse](#math-ResultResponse) | RPC method to multiply the input by 6. |
| Calculate7x | [OperandRequest](#math-OperandRequest) | [ResultResponse](#math-ResultResponse) | RPC method to multiply the input by 7. |
| Calculate8x | [OperandRequest](#math-OperandRequest) | [ResultResponse](#math-ResultResponse) | RPC method to multiply the input by 8. |
| Calculate9x | [OperandRequest](#math-OperandRequest) | [ResultResponse](#math-ResultResponse) | RPC method to multiply the input by 9. |


<a name="math-SqrtService"></a>

### SqrtService
Service that calculates the square root of a number.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| Calculate | [SqrtRequest](#math-SqrtRequest) | [SqrtResponse](#math-SqrtResponse) | RPC method to calculate the square root of a number. The client sends a SqrtRequest, and the server returns the SqrtResponse with the result. |

 



<a name="sum-proto"></a>
<p align="right"><a href="#top">Top</a></p>

## sum.proto



<a name="sum-SumOperand"></a>

### SumOperand
Message containing operands for a sum operation.
Both operands are required for the sum calculation.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| op1 | [int32](#int32) |  | [required] The first operand for the sum. |
| op2 | [int32](#int32) |  | [required] The second operand for the sum. |






<a name="sum-SumResult"></a>

### SumResult
Message containing the result of a sum operation.
The result is always present in the response.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| result | [int32](#int32) |  | [required] The result of the sum operation. |





 

 

 


<a name="sum-SumService"></a>

### SumService
Service that computes the sum of two integers.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| ComputeSum | [SumOperand](#sum-SumOperand) | [SumResult](#sum-SumResult) | RPC method to compute the sum of two integers. The client sends a SumOperand message with two operands, and the server returns a SumResult containing the sum. |

 



## Scalar Value Types

| .proto Type | Notes | C++ | Java | Python | Go | C# | PHP | Ruby |
| ----------- | ----- | --- | ---- | ------ | -- | -- | --- | ---- |
| <a name="double" /> double |  | double | double | float | float64 | double | float | Float |
| <a name="float" /> float |  | float | float | float | float32 | float | float | Float |
| <a name="int32" /> int32 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint32 instead. | int32 | int | int | int32 | int | integer | Bignum or Fixnum (as required) |
| <a name="int64" /> int64 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint64 instead. | int64 | long | int/long | int64 | long | integer/string | Bignum |
| <a name="uint32" /> uint32 | Uses variable-length encoding. | uint32 | int | int/long | uint32 | uint | integer | Bignum or Fixnum (as required) |
| <a name="uint64" /> uint64 | Uses variable-length encoding. | uint64 | long | int/long | uint64 | ulong | integer/string | Bignum or Fixnum (as required) |
| <a name="sint32" /> sint32 | Uses variable-length encoding. Signed int value. These more efficiently encode negative numbers than regular int32s. | int32 | int | int | int32 | int | integer | Bignum or Fixnum (as required) |
| <a name="sint64" /> sint64 | Uses variable-length encoding. Signed int value. These more efficiently encode negative numbers than regular int64s. | int64 | long | int/long | int64 | long | integer/string | Bignum |
| <a name="fixed32" /> fixed32 | Always four bytes. More efficient than uint32 if values are often greater than 2^28. | uint32 | int | int | uint32 | uint | integer | Bignum or Fixnum (as required) |
| <a name="fixed64" /> fixed64 | Always eight bytes. More efficient than uint64 if values are often greater than 2^56. | uint64 | long | int/long | uint64 | ulong | integer/string | Bignum |
| <a name="sfixed32" /> sfixed32 | Always four bytes. | int32 | int | int | int32 | int | integer | Bignum or Fixnum (as required) |
| <a name="sfixed64" /> sfixed64 | Always eight bytes. | int64 | long | int/long | int64 | long | integer/string | Bignum |
| <a name="bool" /> bool |  | bool | boolean | boolean | bool | bool | boolean | TrueClass/FalseClass |
| <a name="string" /> string | A string must always contain UTF-8 encoded or 7-bit ASCII text. | string | String | str/unicode | string | string | string | String (UTF-8) |
| <a name="bytes" /> bytes | May contain any arbitrary sequence of bytes. | string | ByteString | str | []byte | ByteString | string | String (ASCII-8BIT) |

