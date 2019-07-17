using CoreBrewServer.Models;
using System.Collections.Generic;
using System.Linq;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;

namespace CoreBrewServer.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class BrewScheduleController : Controller
    {
        private readonly BrewingContext _context;
        public BrewScheduleController(BrewingContext context){
            _context = context;
        }

        // GET: api/BrewSchedule
        [HttpGet]
        public IEnumerable<BrewSchedule> Get()
        {
            return _context.BrewSchedules
                .Include(bs => bs.BrewDays)
                .ToList();
        }

        // GET: api/BrewSchedule/1
        [HttpGet("{id}")]
        public IActionResult Get(int id)
        {
            BrewSchedule BrewSchedule = _context.BrewSchedules
                .Include(bs => bs.BrewDays)
                .FirstOrDefault(x => x.Id == id);

            return StatusCode(201, BrewSchedule);
        }

        // POST: api/BrewSchedule/
        [HttpPost]
        public IActionResult Post([FromBody] BrewSchedule bd)
        {
            _context.BrewSchedules.Add(bd);
            _context.SaveChanges();
            return StatusCode(201, bd);
        }

        // PUT: api/BrewSchedule/1
        [HttpPut("{id}")]
        public IActionResult Put(int id, [FromBody] BrewSchedule bd)
        {
            _context.BrewSchedules.Update(bd);
            _context.SaveChanges();

            return StatusCode(201, bd);
        }

        // DELETE: api/BrewSchedule/1
        [HttpDelete("{id}")]
        public IActionResult Delete(int id)
        {
            BrewSchedule BrewSchedule = _context.BrewSchedules
                .Include(bs => bs.BrewDays)
                .FirstOrDefault(x => x.Id == id);
            _context.BrewSchedules.Remove(BrewSchedule);
            _context.SaveChanges();
            return StatusCode(201, BrewSchedule);
        }
    }
}