# `protoc-gen-doc` installation

### 1. Install Go
First, you need to have Go installed on your system. If you don't have it yet, you can install it by following these steps:

```bash
sudo apt update
sudo apt install golang-go
```

### 2. Install `protoc-gen-doc`
Once Go is installed, you can use `go install` to install `protoc-gen-doc` directly from the source repository:

```bash
go install github.com/pseudomuto/protoc-gen-doc/cmd/protoc-gen-doc@latest
```

This will download and build the `protoc-gen-doc` binary, placing it in your Go binary directory (typically `$HOME/go/bin`).

### 3. Add Go Bin to PATH
Ensure that Go binaries are in your system's PATH:

```bash
export PATH=$PATH:$HOME/go/bin
```

To make this change permanent, you can add the line above to your `.bashrc` or `.zshrc` file:

```bash
echo 'export PATH=$PATH:$HOME/go/bin' >> ~/.bashrc
source ~/.bashrc
```

### 4. Verify Installation
You can now check if `protoc-gen-doc` is installed correctly by running:

```bash
protoc-gen-doc --version
```

# Writing Documentation for Protocol Buffers (.proto) Files

Documentation is essential for understanding and using API definitions effectively. Here are best practices for documenting `.proto` files to ensure clear and concise API documentation.

## 1. Basic Structure of .proto Files

A typical `.proto` file includes the following components:
- **Syntax Declaration**: Specifies the version of Protocol Buffers.
- **Package Declaration**: Defines a namespace for your messages and services.
- **Messages**: Define data structures.
- **Services**: Define RPC (Remote Procedure Call) methods.

### Example
```proto
syntax = "proto3";

package example;

// Message representing a request.
message ExampleRequest {
    // [required] The unique identifier for the request.
    int32 id = 1;
}

// Message representing a response.
message ExampleResponse {
    // [required] The status of the request processing.
    string status = 1;
}

// Service that processes requests.
service ExampleService {
    // RPC method to process an example request.
    rpc ProcessExample(ExampleRequest) returns (ExampleResponse);
}
```

## 2. Writing Effective Comments

### 2.1 General Comments
- Add a comment for the package declaration to describe the purpose of the package.
- Use comments before each message and service to provide context.

### 2.2 Field Documentation
- Document each field within messages.
- Indicate whether a field is `[required]` or `[optional]` before the description.
- Describe the purpose of the field clearly.

### Example of Field Documentation
```proto
// Message representing a user profile.
message UserProfile {
    // [required] The user's unique identifier.
    int32 user_id = 1;

    // [optional] The user's name.
    string name = 2;

    // [required] The user's email address.
    string email = 3;
}
```

### 2.3 Service and RPC Method Documentation
- Document each service to explain its purpose.
- Describe each RPC method, including the request and response types.
- Provide information on what each method does and any side effects.

### Example of Service Documentation
```proto
// Service for managing user accounts.
service UserService {
    // RPC method to create a new user account.
    // The client sends a CreateUserRequest, and the server returns a CreateUserResponse.
    rpc CreateUser(CreateUserRequest) returns (CreateUserResponse);
}
```

## 3. Using `protoc-gen-doc` for Documentation Generation

### Generating Documentation
You can generate documentation in various formats (Markdown, HTML, DocBook, JSON) using `protoc`.

```bash
# Generate Markdown documentation
protoc --doc_out=./docs --doc_opt=markdown,index.md *.proto

# Generate HTML documentation
protoc --doc_out=./docs --doc_opt=html,index.html *.proto

# Generate DocBook documentation
protoc --doc_out=./docs --doc_opt=docbook,index.docbook *.proto

# Generate JSON documentation
protoc --doc_out=./docs --doc_opt=json,index.json *.proto
```

## 4. Best Practices
- **Be Consistent**: Use consistent terminology and structure throughout your documentation.
- **Keep it Simple**: Write clear, concise descriptions. Avoid jargon when possible.
- **Update Documentation**: Regularly update comments to reflect changes in the API.
- **Use Examples**: Provide usage examples in your comments when appropriate.

