
/*
	当请求到达，使用线程池的一个线程获取句柄，另一个线程处理异步调用
	RCF::RemoteCallContext<> 捕获远程调用服务端上下文，并队列后做后续处理
	RemoteCallContext::parameters() 访问每一个调用的参数
	RemoteCallContext::commit() 发送调用回应
*/