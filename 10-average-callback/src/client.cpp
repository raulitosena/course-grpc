#include <grpcpp/grpcpp.h>
#include <proto/average.pb.h>
#include <proto/average.grpc.pb.h>
#include <condition_variable>


class AverageWriteReactor : public grpc::ClientWriteReactor<average::AvgSample>
{
public:
    AverageWriteReactor(average::AverageService::Stub* stub, const std::vector<int>& samples)
        : stream_samples(samples), current_sample(stream_samples.begin())
	{
        stub->async()->ComputeAvg(&this->context, &this->response, this);
        this->StartCall();
        this->NextWrite();
    }

    void OnWriteDone(bool ok) override
	{
        if (ok)
		{
            // Proceed to write the next sample
            this->NextWrite();
        }
		else
		{
            // Finish writing if not ok (indicating failure or completion)
            this->StartWritesDone();
        }
    }

    void OnDone(const grpc::Status& status) override
	{
        std::unique_lock<std::mutex> lock(this->mtx);
        this->status = status;
        this->done = true;
        this->cv.notify_one();
    }

    grpc::Status Await(average::AvgTotal& response)
	{
        std::unique_lock<std::mutex> lock(this->mtx);
        this->cv.wait(lock, [this] { return this->done; });
        response = std::move(this->response);
        return std::move(this->status);
    }

private:
    void NextWrite()
	{
        if (this->current_sample != this->stream_samples.end())
		{
            // Set the value for the current sample
            this->request.set_value(*this->current_sample);
            // Write the sample asynchronously
            this->StartWrite(&this->request);
            // Move to the next sample
            ++this->current_sample;
        }
		else
		{
            // No more samples to write, signal done
            this->StartWritesDone();
        }
    }

private:
    grpc::ClientContext context;
    average::AvgTotal response;
    average::AvgSample request;
    std::vector<int> stream_samples;
    std::vector<int>::iterator current_sample;
    std::mutex mtx;
    std::condition_variable cv;
    grpc::Status status;
    bool done = false;
};

class AverageClient
{
public:
	explicit AverageClient(std::shared_ptr<grpc::Channel> channel) : stub(average::AverageService::NewStub(channel))
	{
	}

	float ComputeAverage(const std::vector<int>& samples)
	{
		// Start the sending process
		AverageWriteReactor reactor(this->stub.get(), samples);
		// Wait for the response
		average::AvgTotal response;
		grpc::Status status = reactor.Await(response);

		if (status.ok())
		{
			return response.value();
		}
		else
		{
			throw std::runtime_error(status.error_message());
		}
	}

private:
	std::unique_ptr<average::AverageService::Stub> stub;
};

int main(int argc, char** argv)
{
    // Ensure at least 1 argument (port) is provided, but no more than 11 (port + 10 numbers)
    if (argc < 2 || argc > 12)
    {
        std::cerr << "Usage: ./client <port> [numbers... (0 to 10)]" << std::endl;
        return 1001;
    }

    try
    {
        // Parse the port
        int port = std::stoi(argv[1]);
        std::string host = absl::StrFormat("localhost:%d", port);
        auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
        AverageClient client(channel);

        // Parse numbers from command-line arguments (if any) into the samples vector
        std::vector<int> samples;
        for (int i = 2; i < argc; ++i)
        {
            int sample = std::stoi(argv[i]);
            samples.push_back(sample);
        }

        // If no numbers are provided, an empty vector will be passed
        float average = client.ComputeAverage(samples);
        std::cout << "Average: " << average << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }

    return 0;
}
